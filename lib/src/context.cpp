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

void add_noncodeproblem(const RDSegment* seg, MIndex index, usize type) {
    if(seg) {
        state::context->add_problem(
            index, fmt::format("Trying to {} in non-code segment '{}'",
                               get_refname(type), seg->name));
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

Context::Context(RDBuffer* b) { this->program.file = *b; }

void Context::set_userdata(const std::string& k, uptr v) {
    if(k.empty()) {
        spdlog::warn("set_userdata(): Empty userdata");
        return;
    }

    m_database->set_userdata(k, v);
}

Context::~Context() {
    if(state::context == this) rdbuffer_destroy(&this->program.file);
    pm::destroy_instance(this->processorplugin, this->processor);
    pm::destroy_instance(this->loaderplugin, this->loader);
}

tl::optional<uptr> Context::get_userdata(const std::string& k) const {
    return m_database->get_userdata(k);
}

bool Context::try_load(const RDLoaderPlugin* plugin) {
    assume(plugin);

    m_database =
        std::make_unique<Database>(plugin->id, this->program.file.source);
    this->loaderplugin = plugin;
    this->loader = pm::create_instance(plugin);

    if(this->loaderplugin->load &&
       this->loaderplugin->load(this->loader, &this->program.file)) {
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

bool Context::set_function(RDAddress address, usize flags) {
    if(RDSegment* seg = this->program.find_segment(address); seg) {
        if(seg->perm & SP_X) {
            memory::set_flag(seg, address, BF_FUNCTION);

            if(!memory::has_flag(seg, address, BF_CODE))
                this->worker.emulator.enqueue_call(address);

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

void Context::add_ref(RDAddress fromaddr, MIndex toaddr, usize type) {
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
                    this->worker.emulator.enqueue_call(toaddr);
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
                    this->worker.emulator.enqueue_call(toaddr);
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

bool Context::memory_map(RDAddress base, usize size) {
    if(!size) {
        spdlog::error("memory_map: Invalid size");
        return false;
    }

    // this->baseaddress = base;
    // this->program.memory_old = std::make_unique<Memory>(size);

    // Create collected types
    for(const auto& [idx, t] : this->collectedtypes) {
        if(idx >= size) {
            spdlog::warn(
                "Context::memory_map({:#x}, {:#x}): Ignoring type '{}', "
                "out of bounds",
                base, size, this->types.to_string(t));
            continue;
        }

        // this->memory_copy_n(idx, idx, this->types.size_of(t));
        // this->set_type(idx, t, 0);
    }

    this->collectedtypes.clear();
    return true;
}

const Function* Context::find_function(RDAddress address) const {
    if(!this->program.find_segment(address)) return nullptr;

    for(const Function& f : this->functions) {
        if(f.contains(address)) return &f;
    }

    return nullptr;
}

tl::optional<RDAddress> Context::get_address(std::string_view name) const {
    return m_database->get_address(name);
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

        name = prefix + "_" + this->to_hex(address, -1); // TODO: SEGM-BITS!!!
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

        auto nameidx = m_database->get_address(dbname, true);

        if(nameidx && (flags & SN_FORCE)) {
            usize n = 0;

            while(nameidx) {
                dbname = fmt::format("{}_{}", name, ++n);
                nameidx = m_database->get_address(dbname, true);
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
