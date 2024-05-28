#include "context.h"
#include "error.h"
#include "state.h"
#include "utils/utils.h"
#include <climits>
#include <limits>

#if !defined(__has_feature)
#define __has_feature(x) 0
#endif

#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)

extern "C" const char* __lsan_default_options() { // NOLINT
    return "print_suppressions=0";
}

extern "C" const char* __lsan_default_suppressions() { // NOLINT
    return "leak:libpython*.so";
}

#endif

namespace redasm {

namespace {

constexpr std::array<char, 16> INTHEX_TABLE = {'0', '1', '2', '3', '4', '5',
                                               '6', '7', '8', '9', 'A', 'B',
                                               'C', 'D', 'E', 'F'};

usize calculate_bits(RDAddress address) {
    if(address <= std::numeric_limits<u8>::max())
        return 8;
    if(address <= std::numeric_limits<u16>::max())
        return 16;
    if(address <= std::numeric_limits<u32>::max())
        return 32;
    return 64;
}

} // namespace

Context::Context(const std::shared_ptr<AbstractBuffer>& b, const RDLoader* l)
    : loader{l}, file{b} {
    assume(this->loader);
    assume(!state::processors.empty());

    this->processor = &state::processors.front();
    this->availableprocessors.emplace_back(this->processor->name);
}

void Context::set_export(usize idx, const std::string& name) {
    this->memory->at(idx).set(BF_NAME | BF_EXPORT);
    this->database.set_name(idx, name);
}

void Context::set_import(usize idx, const std::string& name) {
    this->memory->at(idx).set(BF_NAME | BF_IMPORT);
    this->database.set_name(idx, name);
}

bool Context::set_type(usize idx, std::string_view tname,
                       const std::string& dbname) {
    auto [type, n] = typing::parse(tname);
    const typing::Type* t = this->types.get_type(type);

    if(!t) {
        spdlog::warn("Type '{}' not found", tname);
        return false;
    }

    bool isarray = n > 0;
    AddressDetail& detail = this->database.get_detail(idx);

    if(t->is_str()) {
        tl::optional<std::string> s;

        if(t->type() == typing::types::WSTR)
            s = this->memory->get_wstring(idx);
        else
            s = this->memory->get_string(idx);

        if(!s)
            return false;

        n = s->size();
        detail.string_length = n + t->size; // Null terminator included
        isarray = false;
    }

    detail.type_name = tname;
    this->memory->set_data(idx, t->size * std::max<usize>(n, 1));
    this->memory->at(idx).set(isarray ? BF_ARRAY : BF_TYPE);

    if(!dbname.empty()) {
        this->set_name(idx, dbname,
                       isarray > 0 ? AddressDetail::ARRAY
                                   : AddressDetail::TYPE);
    }

    return true;
}

void Context::memory_map(RDAddress base, usize size) {
    this->baseaddress = base;
    this->memory = std::make_unique<Memory>(size);
    this->bits = calculate_bits(this->end_baseaddress());

    // Create collected types
    for(const auto& [idx, tname] : this->collectedtypes) {
        if(idx >= size) {
            spdlog::warn("Context::mem_map({:#x}, {:#x}): Ignoring type '{}', "
                         "out of bounds",
                         base, size, tname);
            continue;
        }

        this->memory_copy_n(idx, idx, this->types.size_of(tname));
        this->set_type(idx, tname);
    }

    this->collectedtypes.clear();
}

tl::optional<usize> Context::address_to_index(RDAddress address) const {
    if(address < this->baseaddress)
        return tl::nullopt;

    usize idx = address - this->baseaddress;
    if(idx > this->memory->size())
        return tl::nullopt;

    return idx;
}

tl::optional<RDAddress> Context::index_to_address(usize index) const {
    if(index > this->memory->size())
        return tl::nullopt;
    return this->baseaddress + index;
}

tl::optional<RDOffset> Context::index_to_offset(usize index) const {
    for(const Segment& s : this->segments) {
        if(index >= s.index && index < s.endindex)
            return s.offset + index;
    }

    return tl::nullopt;
}

const Segment* Context::index_to_segment(usize index) const {
    if(index >= state::context->memory->size())
        return {};

    for(const Segment& s : state::context->segments) {
        if(index >= s.index && index < s.endindex)
            return &s;
    }

    return nullptr;
}

bool Context::is_address(RDAddress address) const {
    return this->memory &&
           (address >= this->baseaddress && address < this->end_baseaddress());
}

RDAddress Context::memory_copy(usize idx, RDOffset start, RDOffset end) const {
    assume(start < this->file->size());
    assume(end <= this->file->size());

    RDOffset size = end - start;
    RDAddress endidx = idx + size;
    assume(endidx <= this->memory->size());

    for(usize i = start; i < end; ++i, ++idx) {
        auto b = this->file->get_byte(i);
        assume(b.has_value());
        this->memory->at(idx).set_byte(*b);
    }

    return this->baseaddress + endidx;
}

void Context::map_segment(const std::string& name, usize idx, usize endidx,
                          RDOffset offset, RDOffset endoffset, usize type) {
    if(offset && endoffset)
        this->memory_copy(idx, offset, endoffset);

    this->memory->at(idx).set(BF_SEGMENT);
    this->database.get_detail(idx).segment_index = this->segments.size();

    this->segments.emplace_back(Segment{
        name,
        type,
        idx,
        endidx,
        offset,
        endoffset,
    });

    auto v = *this->index_to_address(endidx);
    this->bits = std::max(this->bits, calculate_bits(v));
}

std::string Context::get_name(usize idx) const {

    Byte b = this->memory->at(idx);
    std::string name;

    if(b.has(BF_NAME)) {
        if(b.has(BF_ARRAY))
            name = this->database.get_name(idx, AddressDetail::ARRAY);
        else if(b.has(BF_TYPE))
            name = this->database.get_name(idx, AddressDetail::TYPE);
        else if(b.has(BF_FUNCTION))
            name = this->database.get_name(idx, AddressDetail::FUNCTION);
        else
            name = this->database.get_name(idx, AddressDetail::LABEL);
    }

    if(name.empty()) {
        std::string prefix = "loc";

        if(b.has(BF_ARRAY) || b.has(BF_TYPE)) {
            const AddressDetail& detail = this->database.get_detail(idx);
            assume(!detail.type_name.empty());

            const typing::Type* t =
                this->types.get_parsed_type(detail.type_name);
            assume(t);
            prefix = utils::to_lower(t->name);
        }
        else if(b.has(BF_FUNCTION))
            prefix = "sub";

        auto address = this->index_to_address(idx);
        assume(address.has_value());
        name = prefix + "_" + this->to_hex(*address);
    }

    return name;
}

void Context::set_name(usize idx, const std::string& name, usize ns) {
    this->memory->at(idx).set(BF_NAME);
    this->database.set_name(idx, name, ns);
}

std::string Context::to_hex(usize v, usize n) const {
    if(!m_nchars)
        m_nchars = (this->bits / CHAR_BIT) * 2;

    if(!n)
        n = m_nchars;

    std::string hexstr;
    hexstr.reserve(n);

    while(v != 0) {
        usize hd = v & 0xF;
        hexstr.insert(0, 1, INTHEX_TABLE[hd]);
        v >>= 4;
    }

    if(hexstr.empty())
        hexstr = "0";
    while(hexstr.size() < n)
        hexstr.insert(0, 1, '0');
    return hexstr;
}

void Context::build_listing() {
    spdlog::info("Building listing...");
    this->listing.clear();

    if(this->memory) {
        for(usize idx = 0; idx < this->memory->size();) {
            Byte membyte = this->memory->at(idx);

            if(membyte.has(BF_SEGMENT)) {
                this->listing.segment(idx);
            }

            if(membyte.is_unknown()) {
                this->process_listing_unknown(idx);
                continue;
            }
            if(membyte.is_data()) {
                this->process_listing_data(idx);
                continue;
            }
            if(membyte.is_code()) {
                this->listing.push_indent(4);
                this->process_listing_code(idx);
                this->listing.pop_indent(4);
                continue;
            }

            unreachable;
        }
    }

    spdlog::info("Listing completed ({} items)", this->listing.size());
}

void Context::process_listing_unknown(usize& idx) {
    usize l = 0, start = idx;

    for(; idx < this->memory->size() && this->memory->at(idx).is_unknown();
        l++, idx++) {
        if(idx != start && this->memory->at(idx).has(BF_SEGMENT))
            break; // Split inter-segment unknowns

        if(l && !(l % 0x10)) {
            this->listing.hex_dump(start, idx);
            start = idx;
        }
    }

    if(idx > start)
        this->listing.hex_dump(start, idx);
}

void Context::process_listing_data(usize& idx) {
    if(this->memory->at(idx).has(BF_ARRAY)) {
        const AddressDetail& d = this->database.get_detail(idx);
        assume(!d.type_name.empty());

        auto [tname, n] = typing::parse(d.type_name);
        assume(n > 0);
        const typing::Type* t = this->types.get_type(tname);
        assume(t);
        this->process_listing_array(idx, t, n);
    }
    else if(this->memory->at(idx).has(BF_TYPE)) {
        const AddressDetail& d = this->database.get_detail(idx);
        assume(!d.type_name.empty());
        auto [tname, n] = typing::parse(d.type_name);
        assume(n == 0);
        const typing::Type* t = this->types.get_type(tname);
        assume(t);
        this->process_listing_type(idx, t);
    }
    else
        idx++;
}

void Context::process_listing_code(usize& idx) {
    if(this->memory->at(idx).has(BF_FUNCTION)) {
        this->listing.pop_indent(2);
        this->listing.function(idx);
        this->listing.push_indent(2);
    }

    this->listing.code(idx++);
    idx += this->memory->get_length(idx);
}

void Context::process_listing_array(usize& idx, const typing::Type* type,
                                    usize n) {
    assume(type);
    assume(n > 0);

    this->listing.array(idx, type->name, n);
    this->listing.push_indent();

    // Array of chars are different
    if(!type->is_char()) {
        for(usize i = 0; i < std::max<usize>(n, 1); i++) {
            usize lidx = this->process_listing_type(idx, type);
            this->listing[lidx].array_index = i;
        }
    }
    else
        idx += type->size * n;

    this->listing.pop_indent();
}

usize Context::process_listing_type(usize& idx, const typing::Type* type) {
    assume(type);

    usize listingidx = this->listing.size();
    this->listing.type(idx, type->name);

    if(type->type() == typing::types::STRUCT) // Struct creates a new context
        this->listing.push_type(type);

    switch(type->type()) {
        case typing::types::CHAR:
        case typing::types::WCHAR:
        case typing::types::I8:
        case typing::types::U8:
        case typing::types::I16:
        case typing::types::U16:
        case typing::types::I32:
        case typing::types::U32:
        case typing::types::I64:
        case typing::types::U64: idx += type->size; break;

        case typing::types::STR:
        case typing::types::WSTR: {
            const AddressDetail& d = this->database.get_detail(idx);
            assume(d.string_length > 0);
            idx += d.string_length;
            break;
        }

        case typing::types::STRUCT: {
            this->listing.push_indent();

            for(usize j = 0; j < type->dict.size(); j++) {
                const auto& item = type->dict[j];
                auto [ftype, n] = typing::parse(item.first);
                const typing::Type* t = this->types.get_type(ftype);
                assume(t);

                this->listing.push_fieldindex(j);

                if(n > 0)
                    this->process_listing_array(idx, t, n);
                else
                    this->process_listing_type(idx, t);

                this->listing.pop_fieldindex();
            }

            this->listing.pop_indent();
            break;
        }

        default: unreachable;
    }

    if(type->type() == typing::types::STRUCT)
        this->listing.pop_type();

    return listingidx;
}

} // namespace redasm
