#include "context.h"
#include "error.h"
#include "state.h"
#include "utils/utils.h"
#include <climits>
#include <limits>
#include <redasm/redasm.h>

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

constexpr std::array<char, 16> INTHEX_TABLE = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};

int calculate_bits(RDAddress address) {
    if(address <= std::numeric_limits<u8>::max())
        return 8;
    if(address <= std::numeric_limits<u16>::max())
        return 16;
    if(address <= std::numeric_limits<u32>::max())
        return 32;
    return 64;
}

} // namespace

Context::Context(const std::shared_ptr<AbstractBuffer>& b, RDLoader* l)
    : loader{l}, file{b} {
    assume(this->loader);
}

bool Context::activate() {
    state::context = this; // Set context as active

    assume(!state::processors.empty());
    this->processor = &state::processors.front();

    this->availableprocessors.emplace_back(this->processor->name);

    if(this->loader->init && this->loader->init(this->loader)) {
        for(const RDAnalyzer& a : state::analyzers) {
            // Assume true if 'isenabled' is not implemented
            if(a.isenabled && !a.isenabled(&a))
                continue;

            this->analyzers.push_back(a); // Take a copy of the analyzer

            if(a.flags & ANALYZER_SELECTED)
                this->selectedanalyzers.insert(a.name);
        }

        return true;
    }

    return false;
}

void Context::set_export(MIndex idx) { // NOLINT
    assume(idx < this->memory->size());
    this->memory->set(idx, BF_EXPORT);
}

void Context::set_import(MIndex idx) { // NOLINT
    assume(idx < this->memory->size());
    this->memory->set(idx, BF_IMPORT);
}

bool Context::set_function(MIndex idx, const std::string& name) {
    if(idx >= this->memory->size() || this->memory->at(idx).has(BF_IMPORT))
        return false;

    const Segment* s = this->index_to_segment(idx);

    if(s && s->type & SEGMENTTYPE_HASCODE) {
        this->memory->set(idx, BF_FUNCTION);
        this->set_name(idx, name);
        return true;
    }

    auto address = this->index_to_address(idx);
    assume(address.has_value());
    spdlog::warn("Address {:x} is not in code segment", *address);
    return false;
}

bool Context::set_entry(MIndex idx, std::string name) {
    if(name.empty())
        name = EP_NAME;

    if(this->set_function(idx, name)) {
        this->set_export(idx);
        this->disassembler.emulator.enqueue(idx);
        this->entrypoint = idx;
        return true;
    }

    return false;
}

bool Context::set_comment(MIndex idx, std::string_view comment) {
    if(idx >= this->memory->size())
        return false;

    AddressDetail& detail = this->database.get_detail(idx);
    detail.comment.assign(comment.begin(), comment.end());
    this->memory->at(idx).set_flag(BF_COMMENT, !comment.empty());
    return true;
}

bool Context::set_type(MIndex idx, std::string_view tname,
                       const std::string& dbname) {
    if(idx >= this->memory->size())
        return false;

    Byte b = this->memory->at(idx);

    if((b.has(BF_ARRAY) || b.has(BF_TYPE)) && !b.is_weak()) {
        const AddressDetail& detail = this->database.get_detail(idx);
        return detail.type_name == tname;
    }
    if(b.is_strong())
        return false;

    auto pt = this->types.parse(tname);

    if(!pt) {
        spdlog::warn("Type '{}' not found", tname);
        return false;
    }

    bool isarray = pt->n > 0;
    AddressDetail& detail = this->database.get_detail(idx);

    usize len;

    if(pt->type->is_str()) {
        tl::optional<std::string> s;

        if(pt->type->id() == typing::types::WSTR)
            s = this->memory->get_wstring(idx);
        else
            s = this->memory->get_string(idx);

        if(!s)
            return false;

        pt->n = s->size();
        detail.string_bytes = (pt->n * pt->type->size) +
                              pt->type->size; // Null terminator included
        isarray = false;

        len = detail.string_bytes;
    }
    else
        len = std::max<usize>(pt->n, 1) * pt->type->size;

    detail.type_name = tname;
    this->memory->set_n(idx, len, BF_DATA);
    this->memory->set(idx, isarray ? BF_ARRAY : BF_TYPE);
    this->set_name(idx, dbname);
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

tl::optional<MIndex> Context::address_to_index(RDAddress address) const {
    if(address < this->baseaddress)
        return tl::nullopt;

    usize idx = address - this->baseaddress;
    if(idx > this->memory->size())
        return tl::nullopt;

    return idx;
}

tl::optional<RDAddress> Context::index_to_address(MIndex index) const {
    if(index > this->memory->size())
        return tl::nullopt;
    return this->baseaddress + index;
}

tl::optional<RDOffset> Context::index_to_offset(MIndex index) const {
    if(index < this->memory->size()) {
        for(const Segment& s : this->segments) {
            if(index >= s.index && index < s.endindex)
                return (index - s.index) + s.offset;
        }
    }

    return tl::nullopt;
}

const Segment* Context::index_to_segment(MIndex index) const {
    if(index >= state::context->memory->size())
        return {};

    for(const Segment& s : this->segments) {
        if(index >= s.index && index < s.endindex)
            return &s;
    }

    return nullptr;
}

const Function* Context::index_to_function(usize index) const {
    if(index >= state::context->memory->size())
        return nullptr;

    for(const Function& f : this->functions) {
        if(f.contains(index))
            return &f;
    }

    return nullptr;
}

bool Context::is_address(RDAddress address) const {
    return this->memory &&
           (address >= this->baseaddress && address < this->end_baseaddress());
}

RDAddress Context::memory_copy(MIndex idx, RDOffset start, RDOffset end) const {
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

void Context::map_segment(const std::string& name, MIndex idx, MIndex endidx,
                          RDOffset offset, RDOffset endoffset, usize type) {
    if(offset && endoffset)
        this->memory_copy(idx, offset, endoffset);

    this->memory->at(idx).set(BF_SEGMENT);

    this->segments.emplace_back(Segment{
        .name = name,
        .type = type,
        .index = idx,
        .endindex = endidx,
        .offset = offset,
        .endoffset = endoffset,
    });

    auto v = *this->index_to_address(endidx);
    this->bits = std::max(this->bits.value_or(0), calculate_bits(v));
}

tl::optional<MIndex> Context::get_index(std::string_view name) const {
    return this->database.get_index(name);
}

std::string Context::get_name(MIndex idx) const {
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

            auto pt = this->types.parse(detail.type_name);
            assume(pt);
            prefix = utils::to_lower(pt->type->name);
        }
        else if(b.has(BF_FUNCTION))
            prefix = "sub";

        auto address = this->index_to_address(idx);
        assume(address.has_value());
        name = prefix + "_" + this->to_hex(*address);
    }

    return name;
}

void Context::set_name(MIndex idx, const std::string& name) {
    if(idx >= this->memory->size())
        return;

    Byte& b = this->memory->at(idx);
    b.set_flag(BF_NAME, !name.empty());

    if(b.has(BF_ARRAY))
        this->database.set_name(idx, name, AddressDetail::ARRAY);
    else if(b.has(BF_TYPE))
        this->database.set_name(idx, name, AddressDetail::TYPE);
    else if(b.has(BF_FUNCTION))
        this->database.set_name(idx, name, AddressDetail::FUNCTION);
    else
        this->database.set_name(idx, name, AddressDetail::LABEL);
}

std::string Context::get_comment(MIndex idx) const {
    if(idx >= this->memory->size() || !this->memory->at(idx).has(BF_COMMENT))
        return {};

    const AddressDetail& detail = this->database.get_detail(idx);
    return detail.comment;
}

std::string Context::to_hex(usize v, int n) const {
    if(!m_nchars && this->bits)
        m_nchars = (*this->bits / CHAR_BIT) * 2;

    if(n == -1)
        n = 0;
    else if(!n)
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
    while(hexstr.size() < static_cast<usize>(n))
        hexstr.insert(0, 1, '0');
    return hexstr;
}

void Context::process_functions() {
    spdlog::info("Processing functions...");
    this->functions.clear();

    if(!this->memory)
        return;

    for(const Segment& seg : this->segments) {
        if(!(seg.type & SEGMENTTYPE_HASCODE))
            continue;

        for(usize idx = seg.index;
            idx < seg.endindex && idx < this->memory->size(); idx++) {
            Byte membyte = this->memory->at(idx);

            if(membyte.has(BF_FUNCTION))
                this->create_function_graph(idx);
        }
    }
}

void Context::process_memory() {
    spdlog::info("Processing memory...");
    this->listing.clear();
    this->functions.clear();

    if(this->memory) {
        for(usize idx = 0; idx < this->memory->size();) {
            Byte membyte = this->memory->at(idx);

            if(membyte.has(BF_SEGMENT))
                this->listing.segment(idx);

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

void Context::process_listing_unknown(MIndex& idx) {
    this->process_hex_dump(idx, [](Byte b) { return b.is_unknown(); });
}

void Context::process_listing_data(MIndex& idx) {
    Byte b = this->memory->at(idx);

    if(b.has(BF_ARRAY)) {
        const AddressDetail& d = this->database.get_detail(idx);
        assume(!d.type_name.empty());

        auto pt = this->types.parse(d.type_name);
        assume(pt);
        assume(pt->n > 0);
        this->process_listing_array(idx, *pt);
    }
    else if(b.has(BF_TYPE)) {
        const AddressDetail& d = this->database.get_detail(idx);
        assume(!d.type_name.empty());

        auto pt = this->types.parse(d.type_name);
        assume(pt);
        assume(pt->n == 0);
        this->process_listing_type(idx, *pt);
    }
    else {
        this->process_hex_dump(idx, [](Byte b) {
            return b.is_data() && !b.has(BF_TYPE) && !b.has(BF_ARRAY);
        });
    }
}

void Context::process_listing_code(MIndex& idx) {
    Byte b = this->memory->at(idx);

    if(b.has(BF_FUNCTION)) {
        this->listing.pop_indent(2);
        this->listing.function(idx);
        this->listing.push_indent(2);

        const Segment* s = this->listing.current_segment();

        if(s && (s->type & SEGMENTTYPE_HASCODE))
            this->create_function_graph(idx);
    }

    if(b.has(BF_JUMPDST)) {
        this->listing.pop_indent();
        this->listing.jump(idx);
        this->listing.push_indent();
    }

    if(b.has(BF_INSTR)) {
        this->listing.instruction(idx);
        idx += this->memory->get_length(idx);
    }
    else {
        this->process_hex_dump(
            idx, [](Byte b) { return b.is_code() && !b.has(BF_INSTR); });
    }
}

void Context::process_listing_array(MIndex& idx, const typing::ParsedType& pt) {
    assume(pt.n > 0);

    this->listing.array(idx, pt);
    this->listing.push_indent();

    // Array of chars are different
    if(!pt.type->is_char()) {
        for(usize i = 0; i < std::max<usize>(pt.n, 1); i++) {
            usize lidx = this->process_listing_type(idx, pt);
            this->listing[lidx].array_index = i;
        }
    }
    else
        idx += pt.type->size * pt.n;

    this->listing.pop_indent();
}

usize Context::process_listing_type(MIndex& idx, const typing::ParsedType& pt) {
    usize listingidx = this->listing.size();
    this->listing.type(idx, pt);

    if(pt.type->id() == typing::types::STRUCT) // Struct creates a new context
        this->listing.push_type(pt);

    switch(pt.type->id()) {
        case typing::types::CHAR:
        case typing::types::WCHAR:
        case typing::types::I8:
        case typing::types::U8:
        case typing::types::I16:
        case typing::types::U16:
        case typing::types::I32:
        case typing::types::U32:
        case typing::types::I64:
        case typing::types::U64: idx += pt.type->size; break;

        case typing::types::STR:
        case typing::types::WSTR: {
            const AddressDetail& d = this->database.get_detail(idx);
            assume(d.string_bytes > 0);
            idx += d.string_bytes;
            break;
        }

        case typing::types::STRUCT: {
            this->listing.push_indent();

            for(usize j = 0; j < pt.type->dict.size(); j++) {
                const auto& item = pt.type->dict[j];
                auto fpt = this->types.parse(item.first);
                assume(fpt);

                this->listing.push_fieldindex(j);

                if(fpt->n > 0)
                    this->process_listing_array(idx, *fpt);
                else
                    this->process_listing_type(idx, *fpt);

                this->listing.pop_fieldindex();
            }

            this->listing.pop_indent();
            break;
        }

        default: unreachable;
    }

    if(pt.type->id() == typing::types::STRUCT)
        this->listing.pop_type();

    return listingidx;
}

void Context::create_function_graph(MIndex idx) {
    auto address = this->index_to_address(idx);
    assume(address.has_value());
    spdlog::info("Creating function graph @ {}", this->to_hex(*address));

    const Database& db = this->database;
    const auto& mem = this->memory;
    Function& f = this->functions.emplace_back(idx);
    std::unordered_set<MIndex> done;
    std::deque<MIndex> pending;
    pending.push_back(idx);

    while(!pending.empty()) {
        MIndex startidx = pending.front();
        pending.pop_front();

        // Ignore loops
        if(done.contains(startidx))
            continue;

        done.insert(startidx);

        RDGraphNode n = f.try_add_block(startidx);

        if(startidx == idx)
            f.graph.set_root(n);

        MIndex curridx = startidx;
        Byte b = mem->at(curridx);

        while(curridx < mem->size()) {
            // Don't overlap functions & blocks
            if((startidx != curridx) && b.has(BF_FUNCTION))
                break;

            // Break loop and connect basic blocks
            if((startidx != curridx) && b.has(BF_JUMPDST)) {
                pending.push_back(curridx);
                f.jmp(n, f.try_add_block(curridx));
                break;
            }

            // ?Delay slots? can have both FLOW and JUMP
            if(b.has(BF_JUMP)) {
                Function::BasicBlock* bb = f.get_basic_block(n);
                assume(bb);
                bb->end = curridx;

                const AddressDetail& d = db.get_detail(curridx);

                for(usize jidx : d.jumps) {
                    const Segment* seg = this->index_to_segment(jidx);

                    if(seg && seg->type & SEGMENTTYPE_HASCODE) {
                        if(!mem->at(jidx).has(BF_INSTR))
                            continue;

                        pending.push_back(jidx);
                        f.jmp_true(n, f.try_add_block(jidx));
                    }
                }
            }

            Function::BasicBlock* bb = f.get_basic_block(n);
            assume(bb);

            if(b.has(BF_FLOW)) {
                const AddressDetail& d = db.get_detail(curridx);

                if(b.has(BF_JUMP)) {
                    pending.push_back(d.flow);
                    f.jmp_false(n, f.try_add_block(d.flow));
                    break;
                }

                bb->end = curridx;
                curridx = d.flow;
                b = mem->at(curridx);
            }
            else {
                bb->end = curridx;
                break;
            }
        }
    }
}

} // namespace redasm
