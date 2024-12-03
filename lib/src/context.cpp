#include "context.h"
#include "api/marshal.h"
#include "error.h"
#include "memory/stringfinder.h"
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

std::string_view get_refname(usize reftype) {
    switch(reftype) {
        case DR_READ: return "READ";
        case DR_WRITE: return "WRITE";
        case DR_ADDRESS: return "ADDRESS";
        case CR_FLOW: return "FLOW";
        case CR_JUMP: return "JUMP";
        case CR_CALL: return "CALL";
        default: break;
    }

    unreachable;
}

void add_noncodeproblem(const Segment* s, MIndex index, usize type) {
    state::context->add_problem(
        index, fmt::format("Trying to {} in non-code segment '{}'",
                           get_refname(type), s->name));
}

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
    : loader{l}, file{b}, m_database{l->id, b->source} {
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

            if(a.flags & ANA_SELECTED)
                this->selectedanalyzers.insert(a.name);
        }

        return true;
    }

    return false;
}

bool Context::set_function(MIndex idx, usize flags) {
    if(const Segment* s = this->index_to_segment(idx); s) {
        if(s->type & SEG_HASCODE) {
            this->memory->set(idx, BF_FUNCTION);

            if(!this->memory->at(idx).has(BF_CODE))
                this->disassembler.emulator.qcall.push_back(idx);

            return true;
        }
    }

    this->add_problem(idx, "Invalid function location");
    return false;
}

bool Context::set_entry(MIndex idx, const std::string& name) {
    if(const Segment* s = this->index_to_segment(idx); s) {
        this->memory->set(idx, BF_EXPORT);

        if(s->type & SEG_HASCODE)
            assume(this->set_function(idx, 0));

        this->set_name(idx, name, 0);
        this->entrypoints.push_back(idx);
        return true;
    }

    this->add_problem(idx, "Invalid entry location");
    return false;
}

void Context::add_ref(MIndex fromidx, MIndex toidx, usize type) {
    if(fromidx >= this->memory->size()) {
        this->add_problem(fromidx, fmt::format("Invalid FROM reference ({:x})",
                                               this->baseaddress + fromidx));
        return;
    }

    const Segment* s = this->index_to_segment(toidx);

    if(!s) {
        this->add_problem(toidx, fmt::format("Invalid {} reference ({:x})",
                                             get_refname(type),
                                             this->baseaddress + toidx));
        return;
    }

    auto& mem = this->memory;

    switch(type) {
        case DR_READ:
        case DR_WRITE: {
            stringfinder::classify(toidx).map([&](const RDStringResult& x) {
                this->memory->unset_n(toidx, x.totalsize);
                this->set_type(toidx, x.type);
            });

            this->m_database.add_ref(fromidx, toidx, type);
            mem->set(fromidx, BF_REFSFROM);
            mem->set(toidx, BF_REFSTO);
            break;
        }

        case DR_ADDRESS: {
            this->m_database.add_ref(fromidx, toidx, type);
            mem->set(fromidx, BF_REFSFROM);
            mem->set(toidx, BF_REFSTO);
            break;
        }

        case CR_FLOW: {
            if(s->type & SEG_HASCODE) {
                if(!mem->at(toidx).has(BF_CODE)) // Check if already decoded
                    this->disassembler.emulator.qflow.push_back(toidx);
                mem->set(fromidx, BF_FLOW);
            }
            else
                add_noncodeproblem(s, toidx, type);
            break;
        }

        case CR_JUMP: {
            this->m_database.add_ref(fromidx, toidx, type);
            mem->set(fromidx, BF_REFSFROM);
            mem->set(toidx, BF_JUMPDST | BF_REFSTO);

            if(s->type & SEG_HASCODE) {
                if(!mem->at(toidx).has(BF_CODE)) // Check if already decoded
                    this->disassembler.emulator.qjump.push_back(toidx);
            }
            else
                add_noncodeproblem(s, toidx, type);
            break;
        }

        case CR_CALL: {
            this->m_database.add_ref(fromidx, toidx, type);
            mem->set(fromidx, BF_REFSFROM);
            mem->set(toidx, BF_FUNCTION | BF_REFSTO);

            if(s->type & SEG_HASCODE) {
                if(!mem->at(toidx).has(BF_CODE)) // Check if already decoded
                    this->disassembler.emulator.qcall.push_back(toidx);
            }
            else
                add_noncodeproblem(s, toidx, type);
            break;
        }

        default: unreachable;
    }
}

bool Context::set_comment(MIndex idx, std::string_view comment) {
    if(idx >= this->memory->size())
        return false;

    this->m_database.set_comment(idx, comment);
    this->memory->at(idx).set_flag(BF_COMMENT, !comment.empty());
    return true;
}

bool Context::set_type(MIndex idx, typing::FullTypeName tname) {
    if(idx >= this->memory->size()) {
        spdlog::warn("set_type: Invalid address");
        return false;
    }

    typing::ParsedType pt = this->types.parse(tname);
    return this->set_type(idx, pt.to_type());
}

bool Context::set_type(MIndex idx, RDType t) {
    if(idx >= this->memory->size()) {
        spdlog::warn("set_type: Invalid address");
        return false;
    }

    const typing::TypeDef* td = this->types.get_typedef(t);
    usize len;

    switch(t.id) {
        case typing::ids::STR:
        case typing::ids::WSTR: {
            tl::optional<std::string> s;
            if(t.id == typing::ids::WSTR)
                s = this->memory->get_wstring(idx);
            else
                s = this->memory->get_string(idx);

            if(!s) {
                spdlog::warn("set_type: string not found in {:x}",
                             this->index_to_address(idx).value());
                return false;
            }

            len = (s->size() * td->size) + td->size; // Null terminator included
            break;
        }

        default: len = std::max<usize>(t.n, 1) * td->size; break;
    }

    m_database.set_type(idx, t);
    this->memory->unset_n(idx, len); // TODO(davide): Handle unset transparently
    this->memory->set_n(idx, len, BF_DATA);
    this->memory->set(idx, BF_TYPE);
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
    if(index >= this->memory->size())
        return {};

    for(const Segment& s : this->segments) {
        if(index >= s.index && index < s.endindex)
            return &s;
    }

    return nullptr;
}

const Function* Context::index_to_function(usize index) const {
    if(index >= this->memory->size())
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
    return m_database.get_index(name);
}

tl::optional<RDType> Context::get_type(MIndex idx) const {
    if(idx >= this->memory->size()) {
        spdlog::warn("get_type: Invalid address");
        return tl::nullopt;
    }

    if(this->memory->at(idx).has(BF_TYPE))
        return m_database.get_type(idx);

    return tl::nullopt;
}

std::string Context::get_name(MIndex idx) const {
    if(idx >= this->memory->size()) {
        spdlog::warn("get_name: Invalid address");
        return {};
    }

    Byte b = this->memory->at(idx);
    std::string name;

    if(b.has(BF_NAME))
        name = m_database.get_name(idx);

    if(name.empty()) {
        std::string prefix = "loc";

        if(b.has(BF_TYPE)) {
            auto type = this->get_type(idx);
            assume(type);

            const typing::TypeDef* td = this->types.get_typedef(*type);
            assume(td);
            prefix = utils::to_lower(td->name);
        }
        else if(b.has(BF_FUNCTION))
            prefix = "sub";

        auto address = this->index_to_address(idx);
        assume(address.has_value());
        name = prefix + "_" + this->to_hex(*address);
    }

    return name;
}

bool Context::set_name(MIndex idx, const std::string& name, usize flags) {
    if(idx >= this->memory->size()) {
        if(!(flags & SN_NOWARN))
            spdlog::warn("set_name: Invalid address");

        return false;
    }

    std::string dbname = name;

    if(!name.empty()) {
        auto nameidx = m_database.get_index(dbname, true);

        if(nameidx && (flags & SN_FORCE)) {
            usize n = 0;

            while(nameidx) {
                dbname = fmt::format("{}_{}", name, ++n);
                nameidx = m_database.get_index(dbname, true);
            }
        }
        else if(nameidx) {
            if(!(flags & SN_NOWARN))
                spdlog::warn("set_name: name '{}' already exists", name);
            return false;
        }
    }

    Byte& b = this->memory->at(idx);
    b.set_flag(BF_NAME, !dbname.empty());
    b.set_flag(BF_IMPORT, flags & SN_IMPORT);

    m_database.set_name(idx, dbname);
    return true;
}

std::string Context::get_comment(MIndex idx) const {
    if(idx >= this->memory->size() || !this->memory->at(idx).has(BF_COMMENT))
        return {};

    return m_database.get_comment(idx);
}

Database::RefList Context::get_refs_from_type(MIndex fromidx,
                                              usize type) const {
    if(fromidx >= this->memory->size() ||
       !this->memory->at(fromidx).has(BF_REFSFROM))
        return {};

    return m_database.get_refs_from_type(fromidx, type);
}

Database::RefList Context::get_refs_from(MIndex fromidx) const {
    if(fromidx >= this->memory->size() ||
       !this->memory->at(fromidx).has(BF_REFSFROM))
        return {};

    return m_database.get_refs_from(fromidx);
}

Database::RefList Context::get_refs_to_type(MIndex toidx, usize type) const {
    if(toidx >= this->memory->size() || !this->memory->at(toidx).has(BF_REFSTO))
        return {};

    return m_database.get_refs_to_type(toidx, type);
}

Database::RefList Context::get_refs_to(MIndex toidx) const {
    if(toidx >= this->memory->size() || !this->memory->at(toidx).has(BF_REFSTO))
        return {};

    return m_database.get_refs_to(toidx);
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

void Context::process_segments() {
    spdlog::info("Processing segments...");
    this->functions.clear();

    if(!this->memory)
        return;

    for(const Segment& seg : this->segments) {
        for(usize idx = seg.index;
            idx < seg.endindex && idx < this->memory->size(); idx++) {
            Byte b = this->memory->at(idx);

            if(b.has(BF_FUNCTION))
                this->process_function_graph(idx);
        }
    }
}

void Context::process_listing() {
    spdlog::info("Processing listing...");
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

    if(b.has(BF_TYPE)) {
        auto type = this->get_type(idx);
        assume(type);

        if(type->n > 0)
            this->process_listing_array(idx, *type);
        else
            this->process_listing_type(idx, *type);
    }
    else
        unreachable;
}

void Context::process_listing_code(MIndex& idx) {
    Byte b = this->memory->at(idx);
    assume(b.has(BF_CODE));

    if(b.has(BF_FUNCTION)) {
        this->listing.pop_indent(2);
        this->listing.function(idx);
        this->listing.push_indent(2);

        const Segment* s = this->listing.current_segment();
        assume(s);
        assume(s->type & SEG_HASCODE);
        this->process_function_graph(idx);
    }
    else if(b.has(BF_REFSTO)) {
        this->listing.pop_indent();
        this->listing.label(idx);
        this->listing.push_indent();
    }

    this->listing.instruction(idx);
    idx += this->memory->get_length(idx);
}

void Context::process_listing_array(MIndex& idx, RDType t) {
    assume(t.n > 0);

    this->listing.type(idx, t);
    this->listing.push_indent();

    const typing::TypeDef* td = this->types.get_typedef(t);
    assume(td);

    switch(td->get_id()) {
        // Array of chars are handled differently
        case typing::ids::CHAR:
        case typing::ids::WCHAR: idx += td->size * t.n; break;

        default: {
            for(usize i = 0; i < std::max<usize>(t.n, 1); i++) {
                LIndex lidx = this->process_listing_type(idx, td->to_type());
                this->listing[lidx].array_index = i;
            }
            break;
        }
    }

    this->listing.pop_indent();
}

LIndex Context::process_listing_type(MIndex& idx, RDType t) {
    LIndex listingidx = this->listing.size();
    this->listing.type(idx, t);

    const typing::TypeDef* td = this->types.get_typedef(t);
    assume(td);

    if(td->is_struct()) { // Struct creates a new scope
        this->listing.push_type(t);
        this->listing.push_indent();

        for(usize j = 0; j < td->dict.size(); j++) {
            const auto& [fieldtype, _] = td->dict[j];

            const typing::TypeDef* ftd = this->types.get_typedef(fieldtype);
            assume(ftd);

            this->listing.push_fieldindex(j);

            if(fieldtype.n > 0)
                this->process_listing_array(idx, ftd->to_type(fieldtype.n));
            else
                this->process_listing_type(idx, ftd->to_type());

            this->listing.pop_fieldindex();
        }

        this->listing.pop_indent();
        this->listing.pop_type();
    }
    else {
        switch(td->get_id()) {
            case typing::ids::CHAR:
            case typing::ids::WCHAR:
            case typing::ids::I8:
            case typing::ids::U8:
            case typing::ids::I16:
            case typing::ids::U16:
            case typing::ids::I32:
            case typing::ids::U32:
            case typing::ids::I64:
            case typing::ids::U64: idx += td->size; break;

            case typing::ids::WSTR:
            case typing::ids::STR: idx += this->memory->get_length(idx); break;

            default: unreachable;
        }
    }

    return listingidx;
}

void Context::process_function_graph(MIndex idx) {
    auto address = this->index_to_address(idx);
    assume(address.has_value());
    spdlog::info("Creating function graph @ {}", this->to_hex(*address));

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

            // FIXME: ?Delay slots? can have both FLOW and JUMP
            if(b.has(BF_JUMP)) {
                Function::BasicBlock* bb = f.get_basic_block(n);
                assume(bb);
                bb->end = curridx;

                for(Database::Ref r :
                    this->get_refs_from_type(curridx, CR_JUMP)) {
                    const Segment* seg = this->index_to_segment(r.index);

                    if(seg && seg->type & SEG_HASCODE) {
                        if(!mem->at(r.index).has(BF_CODE))
                            continue;

                        pending.push_back(r.index);
                        f.jmp_true(n, f.try_add_block(r.index));
                    }
                }
            }

            Function::BasicBlock* bb = f.get_basic_block(n);
            assume(bb);

            if(b.has(BF_FLOW)) {
                usize len = this->memory->get_length(curridx);
                MIndex flow = curridx + len;
                assume(flow > curridx);

                if(b.has(BF_JUMP)) {
                    pending.push_back(flow);
                    f.jmp_false(n, f.try_add_block(flow));
                    break;
                }

                bb->end = curridx;
                curridx = flow;
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
