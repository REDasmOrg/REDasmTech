#include "context.h"
#include "api/marshal.h"
#include "error.h"
#include "plugins/pluginmanager.h"
#include "state.h"
#include "typing/base.h"
#include "utils/utils.h"
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
        case CR_JUMP: return "JUMP";
        case CR_CALL: return "CALL";
        default: break;
    }

    unreachable;
}

void add_noncodeproblem(const Segment* s, MIndex index, usize type) {
    if(s) {
        state::context->add_problem(
            index, fmt::format("Trying to {} in non-code segment '{}'",
                               get_refname(type), s->name));
    }
    else {
        state::context->add_problem(
            index,
            fmt::format("Trying to {} outside of segments", get_refname(type)));
    }
}

constexpr std::array<char, 16> INTHEX_TABLE = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};

} // namespace

Context::Context(const std::shared_ptr<AbstractBuffer>& b): file{b} {}

void Context::set_userdata(const std::string& k, uptr v) {
    if(k.empty()) {
        spdlog::warn("set_userdata(): Empty userdata");
        return;
    }

    m_database->set_userdata(k, v);
}

Context::~Context() {
    pm::destroy_instance(this->processorplugin, this->processor);
    pm::destroy_instance(this->loaderplugin, this->loader);
}

tl::optional<uptr> Context::get_userdata(const std::string& k) const {
    return m_database->get_userdata(k);
}

bool Context::try_load(const RDLoaderPlugin* plugin) {
    assume(plugin);

    m_database = std::make_unique<Database>(plugin->id, this->file->source);
    this->loaderplugin = plugin;
    this->loader = pm::create_instance(plugin);

    if(this->loaderplugin->load &&
       this->loaderplugin->load(this->loader, api::to_c(this->file.get()))) {

        // Select proposed processor
        if(this->loaderplugin->get_processor) {
            const char* p = this->loaderplugin->get_processor(loader);
            if(p) this->processorplugin = pm::find_processor(p);
        }

        if(!this->processorplugin)
            this->processorplugin = pm::find_processor("null");

        assume(this->processorplugin);
        return true;
    }

    return false;
}

void Context::setup(const RDProcessorPlugin* plugin) {
    assume(this->loaderplugin);
    if(plugin) this->processorplugin = plugin;
    assume(this->processorplugin);
    this->processor = pm::create_instance(this->processorplugin);

    foreach_analyzers(ap, {
        // Assume true if 'isenabled' is not implemented
        if(!ap->is_enabled || ap->is_enabled(ap)) {
            this->analyzerplugins.push_back(ap);
            if(ap->flags & AF_SELECTED)
                this->selectedanalyzerplugins.insert(ap);
        }
    });

    this->worker.emulator.setup();
}

bool Context::set_function(MIndex idx, usize flags) {
    if(const Segment* s = this->index_to_segment(idx); s) {
        if(s->type & SEG_HASCODE) {
            this->memory->set(idx, BF_FUNCTION);

            if(!this->memory->at(idx).has(BF_CODE))
                this->worker.emulator.enqueue_call(idx);

            return true;
        }
    }

    this->add_problem(idx, "Invalid function location");
    return false;
}

bool Context::set_entry(MIndex idx, const std::string& name) {
    if(const Segment* s = this->index_to_segment(idx); s) {
        this->memory->set(idx, BF_EXPORT);

        if(s->type & SEG_HASCODE) assume(this->set_function(idx, 0));

        this->set_name(idx, name, 0);
        this->entrypoints.push_back(idx);
        return true;
    }

    this->add_problem(idx, "Invalid entry location");
    return false;
}

void Context::add_ref(MIndex fromidx, MIndex toidx, usize type) {
    if(fromidx >= this->memory->size()) {
        this->add_problem(fromidx,
                          fmt::format("Invalid {} reference (from {:x})",
                                      get_refname(type),
                                      this->baseaddress + fromidx));
        return;
    }

    if(toidx >= this->memory->size()) {
        this->add_problem(fromidx, fmt::format("Invalid {} reference (to {:x})",
                                               get_refname(type),
                                               this->baseaddress + toidx));
        return;
    }

    const Segment* s = this->index_to_segment(toidx);

    switch(type) {
        case DR_READ:
        case DR_WRITE:
        case DR_ADDRESS: {
            this->m_database->add_ref(fromidx, toidx, type);
            this->memory->set(fromidx, BF_REFSFROM);
            this->memory->set(toidx, BF_REFSTO);
            break;
        }

        case CR_JUMP: {
            if(s) {
                this->m_database->add_ref(fromidx, toidx, type);
                this->memory->set(fromidx, BF_REFSFROM);
                this->memory->set(toidx, BF_JUMPDST | BF_REFSTO);
            }

            if(s && s->type & SEG_HASCODE) {
                // Check if already decoded
                if(!this->memory->at(toidx).is_code())
                    this->worker.emulator.enqueue_jump(toidx);
            }
            else
                add_noncodeproblem(s, fromidx, type);
            break;
        }

        case CR_CALL: {
            if(s) {
                this->m_database->add_ref(fromidx, toidx, type);
                this->memory->set(fromidx, BF_REFSFROM);
                this->memory->set(toidx, BF_FUNCTION | BF_REFSTO);
            }

            if(s && s->type & SEG_HASCODE) {
                // Check if already decoded
                if(!this->memory->at(toidx).is_code())
                    this->worker.emulator.enqueue_call(toidx);
            }
            else
                add_noncodeproblem(s, fromidx, type);
            break;
        }

        default: unreachable;
    }
}

bool Context::set_comment(MIndex idx, std::string_view comment) {
    if(idx >= this->memory->size()) return false;

    this->m_database->set_comment(idx, comment);
    this->memory->at(idx).set_flag(BF_COMMENT, !comment.empty());
    return true;
}

bool Context::set_type(MIndex idx, typing::FullTypeName tname, usize flags) {
    if(idx >= this->memory->size()) {
        spdlog::warn("set_type: Invalid address");
        return false;
    }

    typing::ParsedType pt = this->types.parse(tname);
    return this->set_type(idx, pt.to_type(), flags);
}

bool Context::set_type(MIndex idx, RDType t, usize flags) {
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
                s = this->memory->get_wstr(idx);
            else
                s = this->memory->get_str(idx);

            if(!s) {
                this->add_problem(idx, "string not found");
                return false;
            }

            len = (s->size() * td->size) + td->size; // Null terminator included
            break;
        }

        default: len = std::max<usize>(t.n, 1) * td->size; break;
    }

    m_database->set_type(idx, t);
    this->memory->unset_n(idx, len);
    this->memory->set_n(idx, len, BF_DATA);
    this->memory->set(idx, BF_TYPE);
    this->memory->set_flags(idx, BF_WEAK, flags & ST_WEAK);
    return true;
}

bool Context::memory_map(RDAddress base, usize size) {
    if(!size) {
        spdlog::error("memory_map: Invalid size");
        return false;
    }

    this->baseaddress = base;
    this->memory = std::make_unique<Memory>(size);

    // Create collected types
    for(const auto& [idx, t] : this->collectedtypes) {
        if(idx >= size) {
            spdlog::warn(
                "Context::memory_map({:#x}, {:#x}): Ignoring type '{}', "
                "out of bounds",
                base, size, this->types.to_string(t));
            continue;
        }

        this->memory_copy_n(idx, idx, this->types.size_of(t));
        this->set_type(idx, t, 0);
    }

    this->collectedtypes.clear();
    return true;
}

tl::optional<MIndex> Context::address_to_index(RDAddress address) const {
    if(address < this->baseaddress) return tl::nullopt;

    usize idx = address - this->baseaddress;
    if(idx >= this->memory->size()) return tl::nullopt;

    return idx;
}

tl::optional<RDAddress> Context::index_to_address(MIndex index) const {
    if(index > this->memory->size()) return tl::nullopt;
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
    if(index >= this->memory->size()) return nullptr;

    const Segment* ls = m_lastsegment;
    if(ls && (index >= ls->index && index < ls->endindex)) return ls;

    for(const Segment& s : this->segments) {
        if(index >= s.index && index < s.endindex) {
            ls = &s;
            return ls;
        }
    }

    return nullptr;
}

const Function* Context::index_to_function(usize index) const {
    if(index >= this->memory->size()) return nullptr;

    for(const Function& f : this->functions) {
        if(f.contains(index)) return &f;
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
    if(idx >= endidx) {
        spdlog::error("Invalid address range for segment '{}'", name);
        return;
    }

    if(idx >= this->memory->size()) {
        spdlog::error("Start address out of range for segment '{}'", name);
        return;
    }

    if(endidx > this->memory->size()) {
        spdlog::error("End address out of range for segment '{}'", name);
        return;
    }

    if(offset != endoffset) {
        if(offset > endoffset) {
            spdlog::error("Invalid offset range for segment '{}'", name);
            return;
        }

        if(offset >= this->file->size()) {
            spdlog::error("Start offset out of range for segment '{}'", name);
            return;
        }

        if(endoffset > this->file->size()) {
            spdlog::error("End offset out of range for segment '{}'", name);
            return;
        }

        this->memory_copy(idx, offset, endoffset);
    }

    this->memory->at(idx).set(BF_SEGMENT);

    this->segments.emplace_back(Segment{
        .name = name,
        .type = type,
        .index = idx,
        .endindex = endidx,
        .offset = offset,
        .endoffset = endoffset,
    });

    m_database->add_segment(name, idx, endidx, offset, endoffset, type);
}

tl::optional<MIndex> Context::get_index(std::string_view name) const {
    return m_database->get_index(name);
}

tl::optional<RDType> Context::get_type(MIndex idx) const {
    if(idx >= this->memory->size()) {
        spdlog::warn("get_type: Invalid address");
        return tl::nullopt;
    }

    if(this->memory->at(idx).has(BF_TYPE)) return m_database->get_type(idx);

    return tl::nullopt;
}

std::string Context::get_name(MIndex idx) const {
    if(idx >= this->memory->size()) {
        spdlog::warn("get_name: Invalid address");
        return {};
    }

    Byte b = this->memory->at(idx);
    std::string name;

    if(b.has(BF_NAME)) name = m_database->get_name(idx);

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
        name = prefix + "_" + this->to_hex(*address, -1);
    }

    return name;
}

bool Context::set_name(MIndex idx, const std::string& name, usize flags) {
    if(idx >= this->memory->size()) {
        if(!(flags & SN_NOWARN))
            this->add_problem(idx, "Cannot set name, address out of bounds");

        return false;
    }

    std::string dbname = name;

    if(!name.empty()) {
        if(this->memory->at(idx).has(BF_NAME)) {
            if(!(flags & SN_NOWARN)) {
                auto addr = this->index_to_address(idx);
                assume(addr);
                this->add_problem(
                    idx,
                    fmt::format("Name already set @ {:x} (trying to set '{}')",
                                *addr, name));
            }
            return false;
        }

        auto nameidx = m_database->get_index(dbname, true);

        if(nameidx && (flags & SN_FORCE)) {
            usize n = 0;

            while(nameidx) {
                dbname = fmt::format("{}_{}", name, ++n);
                nameidx = m_database->get_index(dbname, true);
            }
        }
        else if(nameidx) {
            if(!(flags & SN_NOWARN)) {
                this->add_problem(
                    idx, fmt::format("name '{}' already exists", name));
            }
            return false;
        }
    }

    Byte& b = this->memory->at(idx);
    b.set_flag(BF_NAME, !dbname.empty());
    b.set_flag(BF_IMPORT, flags & SN_IMPORT);

    m_database->set_name(idx, dbname);
    return true;
}

std::string Context::get_comment(MIndex idx) const {
    if(idx >= this->memory->size() || !this->memory->at(idx).has(BF_COMMENT))
        return {};

    return m_database->get_comment(idx);
}

Database::RefList Context::get_refs_from_type(MIndex fromidx,
                                              usize type) const {
    if(fromidx >= this->memory->size() ||
       !this->memory->at(fromidx).has(BF_REFSFROM))
        return {};

    return m_database->get_refs_from_type(fromidx, type);
}

Database::RefList Context::get_refs_from(MIndex fromidx) const {
    if(fromidx >= this->memory->size() ||
       !this->memory->at(fromidx).has(BF_REFSFROM))
        return {};

    return m_database->get_refs_from(fromidx);
}

Database::RefList Context::get_refs_to_type(MIndex toidx, usize type) const {
    if(toidx >= this->memory->size() || !this->memory->at(toidx).has(BF_REFSTO))
        return {};

    return m_database->get_refs_to_type(toidx, type);
}

Database::RefList Context::get_refs_to(MIndex toidx) const {
    if(toidx >= this->memory->size() || !this->memory->at(toidx).has(BF_REFSTO))
        return {};

    return m_database->get_refs_to(toidx);
}

std::string Context::to_hex(usize v, int n) const {
    if(n == -1) n = 0;

    std::string hexstr;
    hexstr.reserve(n);

    while(v != 0) {
        usize hd = v & 0xF;
        hexstr.insert(0, 1, INTHEX_TABLE[hd]);
        v >>= 4;
    }

    if(hexstr.empty()) hexstr = "0";
    while(hexstr.size() < static_cast<usize>(n))
        hexstr.insert(0, 1, '0');
    return hexstr;
}

} // namespace redasm
