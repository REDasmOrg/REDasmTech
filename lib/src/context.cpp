#include "context.h"
#include "api/marshal.h"
#include "memory/memory.h"
#include "plugins/pluginmanager.h"
#include "state.h"
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

    ct_unreachable;
}

void add_noncodeproblem(const RDSegment* seg, RDAddress address, usize type) {
    if(seg) {
        state::context->add_problem(
            address, fmt::format("Trying to {} in non-code segment '{}'",
                                 get_refname(type), seg->name));
    }
    else {
        state::context->add_problem(
            address,
            fmt::format("Trying to {} outside of segments", get_refname(type)));
    }
}

} // namespace

Context::Context(RDBuffer* file) {
    slice_init(&this->problems, nullptr, nullptr);
    this->program.file = file;
}

Context::~Context() {
    delete m_database;
    delete this->worker;

    if(state::context == this) rdbuffer_destroy(this->program.file);
    pm::destroy_instance(this->processorplugin, this->processor);
    pm::destroy_instance(this->loaderplugin, this->loader);

    RDProblem* p;
    slice_foreach(p, &this->problems) delete[] p->problem;
    slice_destroy(&this->problems);
}

bool Context::parse(const RDLoaderPlugin* plugin, const RDLoaderRequest* req) {
    ct_assume(plugin->parse);
    this->loaderplugin = plugin;
    this->loader = pm::create_instance(plugin);

    if(this->loaderplugin->parse(this->loader, req)) {
        const RDProcessorPlugin* pp = nullptr;

        if(plugin->get_processor) {
            const char* p = plugin->get_processor(this->loader);
            if(p) pp = pm::find_processor(p);
        }

        if(!pp) pp = pm::find_processor("null");
        ct_assume(pp);
        this->processorplugin = pp;
        return true;
    }

    return false;
}

bool Context::load(const RDProcessorPlugin* plugin) {
    if(!plugin) plugin = pm::find_processor("null");
    this->processorplugin = plugin;
    ct_assume(this->loaderplugin);
    ct_assume(this->processorplugin);

    m_database = new Database{plugin->id, this->program.file->source};
    this->processor = pm::create_instance(plugin);
    this->worker = new Worker{};

    if(this->loaderplugin->load &&
       this->loaderplugin->load(this->loader, this->program.file)) {
        const RDAnalyzerPlugin** p;
        slice_foreach(p, &pm::analyzers) {
            // Assume true if 'isenabled' is not implemented
            if(!(*p)->is_enabled || (*p)->is_enabled((*p))) {
                this->analyzerplugins.push_back((*p));
                if((*p)->flags & AF_SELECTED)
                    this->selectedanalyzerplugins.insert((*p));
            }
        }

        // Sort analyzers by order
        std::ranges::sort(this->analyzerplugins,
                          [](const auto* lhs, const auto* rhs) {
                              return lhs->order < rhs->order;
                          });

        if(this->loaderplugin->load_signatures) {
            this->loaderplugin->load_signatures(this->loader,
                                                api::to_c(&this->signatures));
        }

        this->worker->emulator.setup();
        return true;
    }

    return false;
}

void Context::set_userdata(const std::string& k, uptr v) {
    if(k.empty()) {
        spdlog::warn("set_userdata(): Empty userdata");
        return;
    }

    m_database->set_userdata(k, v);
}

tl::optional<uptr> Context::get_userdata(const std::string& k) const {
    return m_database->get_userdata(k);
}

bool Context::set_function(RDAddress address, usize flags) {
    address = this->normalize_address(address, false);

    if(RDSegment* seg = this->program.find_segment(address); seg) {
        if(seg->perm & SP_X) {
            memory::set_flag(seg, address, BF_FUNCTION);

            if(!memory::has_flag(seg, address, BF_CODE))
                this->worker->emulator.enqueue_call(address);

            if(flags & SF_ENTRY) this->entrypoint = address;
            return true;
        }
    }

    this->add_problem(address, "Invalid function location");
    return false;
}

bool Context::set_entry(RDAddress address, const std::string& name) {
    if(this->set_function(address, SF_ENTRY))
        return this->set_name(address, name, SN_EXPORT);

    this->add_problem(address, "Invalid entry location");
    return false;
}

void Context::add_ref(RDAddress fromaddr, RDAddress toaddr, usize type) {
    fromaddr = this->normalize_address(fromaddr);
    toaddr = this->normalize_address(toaddr);
    RDSegment* fromseg = this->program.find_segment(fromaddr);
    RDSegment* toseg = this->program.find_segment(toaddr);

    if(!fromseg) {
        this->add_problem(fromaddr, fmt::format("Invalid FROM {} reference",
                                                get_refname(type), fromaddr));
        return;
    }

    if(!toseg) {
        this->add_problem(
            toaddr, fmt::format("Invalid TO {} reference", get_refname(type)));
        return;
    }

    switch(type) {
        case DR_READ:
        case DR_WRITE:
        case DR_ADDRESS: {
            this->m_database->add_ref(fromaddr, toaddr, type);
            memory::set_flag(fromseg, fromaddr, BF_REFSFROM);
            memory::set_flag(toseg, toaddr, BF_REFSTO);
            break;
        }

        case CR_JUMP: {
            this->m_database->add_ref(fromaddr, toaddr, type);
            memory::set_flag(fromseg, fromaddr, BF_REFSFROM);
            memory::set_flag(toseg, toaddr, BF_JUMPDST | BF_REFSTO);

            if(toseg->perm & SP_X) {
                // Check if already decoded
                if(!memory::has_flag(toseg, toaddr, BF_CODE))
                    this->worker->emulator.enqueue_call(toaddr);
            }
            else
                add_noncodeproblem(toseg, fromaddr, type);
            break;
        }

        case CR_CALL: {
            this->m_database->add_ref(fromaddr, toaddr, type);
            memory::set_flag(fromseg, fromaddr, BF_REFSFROM);
            memory::set_flag(toseg, toaddr, BF_FUNCTION | BF_REFSTO);

            if(toseg->perm & SP_X) {
                // Check if already decoded
                if(!memory::has_flag(toseg, toaddr, BF_CODE))
                    this->worker->emulator.enqueue_call(toaddr);
            }
            else
                add_noncodeproblem(toseg, toaddr, type);
            break;
        }

        default: ct_unreachable;
    }
}

bool Context::set_comment(RDAddress address, std::string_view comment) {
    RDSegment* seg = this->program.find_segment(address);
    if(!seg) return false;

    this->m_database->set_comment(address, comment);
    memory::set_flag(seg, address, BF_CODE, !comment.empty());
    return true;
}

bool Context::set_type(RDAddress address, std::string_view tname, usize flags) {
    auto t = this->types.create_type(tname);
    return t && this->set_type(address, *t, flags);
}

bool Context::set_type(RDAddress address, RDType t, usize flags) {
    RDSegment* seg = this->program.find_segment(address);
    if(!seg) {
        spdlog::warn("set_type: Invalid address");
        return false;
    }

    if(!t.def) {
        spdlog::warn("set_type: Invalid type definition");
        return false;
    }

    tl::optional<std::string> s;
    tl::optional<RDLEB128> leb;
    usize len;

    if(t.def->kind == TK_PRIMITIVE) {
        switch(t.def->t_primitive) {
            case T_STR:
                s = memory::get_str(seg, address);
                ct_assume(s);
                break;

            case T_WSTR:
                s = memory::get_wstr(seg, address);
                ct_assume(s);
                break;

            case T_LEB128:
                leb = memory::get_leb128(seg, address);
                ct_assume(leb);
                break;

            case T_ULEB128:
                leb = memory::get_uleb128(seg, address);
                ct_assume(leb);
                break;

            default: break;
        }
    }

    if(s) { // Set null terminator as part of the string
        len = (s->size() * t.def->size) + t.def->size;
    }
    else if(leb)
        len = leb->size;
    else
        len = this->types.size_of(t);

    m_database->set_type(address, t);
    memory::unset_n(seg, address, len);
    memory::set_n(seg, address, len, BF_DATA);
    memory::set_flag(seg, address, BF_TYPE);
    memory::set_flag(seg, address, BF_WEAK, flags & ST_WEAK);
    return true;
}

tl::optional<RDAddress> Context::get_address(std::string_view name,
                                             bool onlydb) const {
    auto addr = m_database->get_address(name);
    if(addr) return addr;

    if(!onlydb) { // Try to extract address from name
        usize idx = name.size();

        while(idx-- > 0) {
            if(!std::isxdigit(name[idx])) break;
        }

        if(idx >= name.size() || name.at(idx) != '_') return tl::nullopt;

        if(++idx < name.size()) {
            return utils::to_integer<RDAddress>(
                std::string_view{name.data() + idx}, 16);
        }
    }

    return tl::nullopt;
}

Database::SRegChanges Context::get_sregs_from_addr(RDAddress address) const {
    const RDSegment* seg = this->program.find_segment(address);
    if(!seg || memory::has_flag(seg, address, BF_SREG)) return {};
    return m_database->get_sregs_from_addr(address);
}

Database::SRegChanges Context::get_sreg_changes(int sreg) const {
    return m_database->get_sreg_changes(sreg);
}

tl::optional<u64> Context::get_sreg(RDAddress address, int sreg) const {
    const RDSRange* r = this->program.find_sreg_range(address, sreg);
    if(r && r->val.ok) return r->val.value;
    return tl::nullopt;
}

Database::SRegList Context::get_sregs() const {
    return m_database->get_sregs();
}

RDAddress Context::normalize_address(RDAddress address, bool query) const {
    ct_assume(this->processorplugin);

    if(this->processorplugin->normalize_address)
        return this->processorplugin->normalize_address(this->processor,
                                                        address, query);

    return address;
}

void Context::set_sreg(RDAddress address, int sreg, const RDRegValue& val,
                       const tl::optional<RDAddress>& fromaddr) {
    if(this->program.set_sreg(address, sreg, val))
        m_database->set_sreg(address, sreg, val, fromaddr);
}

tl::optional<RDType> Context::get_type(RDAddress address) const {
    const RDSegment* seg = this->program.find_segment(address);

    if(!seg) {
        spdlog::warn("get_type: Invalid address");
        return {};
    }

    if(memory::has_flag(seg, address, BF_TYPE)) {
        auto dbtype = m_database->get_type(address);
        if(dbtype) return this->types.create_type(dbtype->name, dbtype->n);
    }

    return tl::nullopt;
}

std::string Context::get_name(RDAddress address, bool autoname) const {
    const RDSegment* seg = this->program.find_segment(address);

    if(!seg) {
        spdlog::warn("get_name: Invalid address");
        return {};
    }

    std::string name;

    if(memory::has_flag(seg, address, BF_NAME))
        name = m_database->get_name(address);

    if(autoname && name.empty()) {
        std::string prefix = "loc";

        if(memory::has_flag(seg, address, BF_TYPE)) {
            auto type = this->get_type(address);
            ct_assume(type);
            ct_assume(type->def);
            prefix = utils::to_lower(type->def->name);
        }
        else if(memory::has_flag(seg, address, BF_FUNCTION))
            prefix = "sub";

        name = prefix + "_" + utils::to_hex<std::string>(address, seg->bits);
    }

    return name;
}

bool Context::set_name(RDAddress address, const std::string& name,
                       usize flags) {
    address = this->normalize_address(address);
    RDSegment* seg = this->program.find_segment(address);

    if(!seg) {
        if(!(flags & SN_NOWARN))
            this->add_problem(address,
                              "Cannot set name, address out of bounds");

        return false;
    }

    std::string dbname = name;

    if(flags & SN_ADDRESS) {
        dbname.append("_");
        dbname.append(utils::to_hex(address, seg->bits));
    }

    if(!name.empty()) {
        if(memory::has_flag(seg, address, BF_NAME)) {
            if(!(flags & SN_NOWARN)) {
                this->add_problem(
                    address,
                    fmt::format("Name already set @ {:x} (trying to set '{}')",
                                address, name));
            }
            return false;
        }

        auto nameidx = this->get_address(dbname, true);

        if(nameidx && (flags & SN_FORCE)) {
            usize n = 0;

            while(nameidx) {
                dbname = fmt::format("{}_{}", name, ++n);
                nameidx = this->get_address(dbname, true);
            }
        }
        else if(nameidx) {
            if(!(flags & SN_NOWARN)) {
                this->add_problem(
                    address, fmt::format("name '{}' already exists", name));
            }
            return false;
        }
    }

    memory::set_flag(seg, address, BF_EXPORT, flags & SN_EXPORT);
    memory::set_flag(seg, address, BF_IMPORT, flags & SN_IMPORT);
    memory::set_flag(seg, address, BF_NAME, !dbname.empty());
    m_database->set_name(address, dbname);
    return true;
}

std::string Context::get_comment(RDAddress address) const {
    const RDSegment* seg = this->program.find_segment(address);
    if(!seg || !memory::has_flag(seg, address, BF_COMMENT)) return {};
    return m_database->get_comment(address);
}

Database::RefList Context::get_refs_from_type(RDAddress fromaddr,
                                              usize type) const {
    const RDSegment* seg = this->program.find_segment(fromaddr);
    if(!seg || !memory::has_flag(seg, fromaddr, BF_REFSFROM)) return {};
    return m_database->get_refs_from_type(fromaddr, type);
}

Database::RefList Context::get_refs_from(RDAddress fromaddr) const {
    const RDSegment* seg = this->program.find_segment(fromaddr);
    if(!seg || !memory::has_flag(seg, fromaddr, BF_REFSFROM)) return {};
    return m_database->get_refs_from(fromaddr);
}

Database::RefList Context::get_refs_to_type(RDAddress toaddr,
                                            usize type) const {
    const RDSegment* seg = this->program.find_segment(toaddr);
    if(!seg || !memory::has_flag(seg, toaddr, BF_REFSTO)) return {};
    return m_database->get_refs_to_type(toaddr, type);
}

Database::RefList Context::get_refs_to(RDAddress toaddr) const {
    const RDSegment* seg = this->program.find_segment(toaddr);
    if(!seg || !memory::has_flag(seg, toaddr, BF_REFSTO)) return {};
    return m_database->get_refs_to(toaddr);
}

void Context::add_problem(RDAddress address, std::string_view s) {
    spdlog::warn("add_problem(): {:x} = {}", address, s);
    slice_push(&this->problems, {address, utils::copy_str(s)});
}

bool Context::add_segment(std::string_view name, RDAddress start, RDAddress end,
                          u32 perm, u32 bits) {
    if(!this->program.add_segment(name, start, end, perm, bits)) return false;

    // Try to initialize segment registers for this segment
    const RDProcessorPlugin* pp = this->processorplugin;

    if((perm & SP_X) && pp && pp->get_segmentregisters) {
        const int* sregs = pp->get_segmentregisters(this->processor);

        while(sregs && *sregs != -1)
            this->program.add_sreg_range(start, end, *sregs++, 0);
    }

    return true;
}

} // namespace redasm
