#include "context.h"
#include "api/marshal.h"
#include "error.h"
#include "memory/memory.h"
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
    this->problems = vect_create(RDProblem);

    vect_setitemdel(this->problems, [](void* x) {
        delete[] reinterpret_cast<RDProblem*>(x)->problem;
    });

    this->program.file = file;
}

Context::~Context() {
    delete m_database;
    delete this->worker;

    if(state::context == this) rdbuffer_destroy(this->program.file);

    pm::destroy_instance(this->processorplugin, this->processor);
    pm::destroy_instance(this->loaderplugin, this->loader);
    vect_destroy(this->problems);
}

bool Context::parse(const RDLoaderPlugin* plugin, const RDLoaderRequest* req) {
    assume(plugin->parse);
    this->loaderplugin = plugin;
    this->loader = pm::create_instance(plugin);

    if(this->loaderplugin->parse(this->loader, req)) {
        const RDProcessorPlugin* pp = nullptr;

        if(plugin->get_processor) {
            const char* p = plugin->get_processor(this->loader);
            if(p) pp = pm::find_processor(p);
        }

        if(!pp) pp = pm::find_processor("null");
        assume(pp);
        this->processorplugin = pp;
        return true;
    }

    return false;
}

bool Context::load(const RDProcessorPlugin* plugin) {
    if(!plugin) plugin = pm::find_processor("null");
    this->processorplugin = plugin;
    assume(this->loaderplugin);
    assume(this->processorplugin);

    m_database = new Database{plugin->id, this->program.file->source};
    this->processor = pm::create_instance(plugin);
    this->worker = new Worker{};

    if(this->loaderplugin->load &&
       this->loaderplugin->load(this->loader, this->program.file)) {
        vect_foreach(const RDAnalyzerPlugin*, p, pm::analyzers) {
            const RDAnalyzerPlugin* ap = *p;

            // Assume true if 'isenabled' is not implemented
            if(!ap->is_enabled || ap->is_enabled(ap)) {
                this->analyzerplugins.push_back(ap);
                if(ap->flags & AF_SELECTED)
                    this->selectedanalyzerplugins.insert(ap);
            }
        }

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
    if(RDSegment* seg = this->program.find_segment(address); seg) {
        if(seg->perm & SP_X) {
            memory::set_flag(seg, address, BF_FUNCTION);

            if(!memory::has_flag(seg, address, BF_CODE))
                this->worker->emulator.enqueue_call(address);

            return true;
        }
    }

    this->add_problem(address, "Invalid function location");
    return false;
}

bool Context::set_entry(RDAddress address, const std::string& name) {
    if(RDSegment* seg = this->program.find_segment(address); seg) {
        memory::set_flag(seg, address, BF_EXPORT);
        if(seg->perm & SP_X) assume(this->set_function(address, 0));
        this->set_name(address, name, 0);
        this->entrypoints.push_back(address);
        return true;
    }

    this->add_problem(address, "Invalid entry location");
    return false;
}

void Context::add_ref(RDAddress fromaddr, RDAddress toaddr, usize type) {
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

        default: unreachable;
    }
}

bool Context::set_comment(RDAddress address, std::string_view comment) {
    RDSegment* seg = this->program.find_segment(address);
    if(!seg) return false;

    this->m_database->set_comment(address, comment);
    memory::set_flag(seg, address, BF_CODE, !comment.empty());
    return true;
}

bool Context::set_type(RDAddress address, typing::FullTypeName tname,
                       usize flags) {
    const RDSegment* seg = this->program.find_segment(address);
    if(!seg) {
        spdlog::warn("set_type: Invalid address");
        return false;
    }

    typing::ParsedType pt = this->types.parse(tname);
    return this->set_type(address, pt.to_type(), flags);
}

bool Context::set_type(RDAddress address, RDType t, usize flags) {
    RDSegment* seg = this->program.find_segment(address);
    if(!seg) {
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
                s = memory::get_wstr(seg, address);
            else
                s = memory::get_str(seg, address);

            if(!s) {
                this->add_problem(address, "string not found");
                return false;
            }

            len = (s->size() * td->size) + td->size; // Null terminator included
            break;
        }

        default: len = std::max<usize>(t.n, 1) * td->size; break;
    }

    m_database->set_type(address, t);
    memory::unset_n(seg, address, len);
    memory::set_n(seg, address, len, BF_DATA);
    memory::set_flag(seg, address, BF_TYPE);
    memory::set_flag(seg, address, BF_WEAK, flags & ST_WEAK);
    return true;
}

const Function* Context::find_function(RDAddress address) const {
    if(!this->program.find_segment(address)) return nullptr;

    for(const Function& f : this->functions) {
        if(f.contains(address)) return &f;
    }

    return nullptr;
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

tl::optional<RDType> Context::get_type(RDAddress address) const {
    const RDSegment* seg = this->program.find_segment(address);

    if(!seg) {
        spdlog::warn("get_name: Invalid address");
        return {};
    }

    if(memory::has_flag(seg, address, BF_TYPE))
        return m_database->get_type(address);

    return tl::nullopt;
}

std::string Context::get_name(RDAddress address) const {
    const RDSegment* seg = this->program.find_segment(address);

    if(!seg) {
        spdlog::warn("get_name: Invalid address");
        return {};
    }

    std::string name;

    if(memory::has_flag(seg, address, BF_NAME))
        name = m_database->get_name(address);

    if(name.empty()) {
        std::string prefix = "loc";

        if(memory::has_flag(seg, address, BF_TYPE)) {
            auto type = this->get_type(address);
            assume(type);

            const typing::TypeDef* td = this->types.get_typedef(*type);
            assume(td);
            prefix = utils::to_lower(td->name);
        }
        else if(memory::has_flag(seg, address, BF_FUNCTION))
            prefix = "sub";

        name = prefix + "_" + utils::to_hex<std::string>(address, seg->bits);
    }

    return name;
}

bool Context::set_name(RDAddress address, const std::string& name,
                       usize flags) {
    RDSegment* seg = this->program.find_segment(address);

    if(!seg) {
        if(!(flags & SN_NOWARN))
            this->add_problem(address,
                              "Cannot set name, address out of bounds");

        return false;
    }

    std::string dbname = name;

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

    memory::set_flag(seg, address, BF_NAME, !dbname.empty());
    memory::set_flag(seg, address, BF_IMPORT, flags & SN_IMPORT);
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
    vect_add(RDProblem, this->problems, {address, utils::copy_str(s)});
}

} // namespace redasm
