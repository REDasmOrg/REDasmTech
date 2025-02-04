#include "../context.h"
#include "../memory/memory.h"
#include "../plugins/modulemanager.h"
#include "../plugins/pluginmanager.h"
#include "../state.h"
#include "../surface/surface.h"
#include "../utils/utils.h"
#include "marshal.h"
#include <algorithm>
#include <cctype>
#include <redasm/redasm.h>
#include <spdlog/spdlog.h>

bool rd_init(const RDInitParams* params) {
    spdlog::trace("rd_init({})", fmt::ptr(params));

    if(redasm::state::initialized) return true;
    redasm::state::initialized = true;

    // clang-format off
    redasm::state::params = {
        .onlog    = [](const char* arg, void*) { spdlog::info("LOG: {}", arg); },
        .onstatus = [](const char* arg, void*) { spdlog::info("STATUS: {}", arg); },
        .onerror  = [](const char* arg, void*) { spdlog::error("ERROR: {}", arg); },
        .userdata = nullptr,
    };
    // clang-format on

    if(params) {
        if(params->onlog) redasm::state::params.onlog = params->onlog;
        if(params->onstatus) redasm::state::params.onstatus = params->onstatus;
        if(params->onerror) redasm::state::params.onerror = params->onerror;
        redasm::state::params.ui = params->ui,
        redasm::state::params.userdata = params->userdata;
    }

    redasm::pm::create();
    return true;
}

void rd_deinit() {
    spdlog::trace("rd_deinit()");
    if(!redasm::state::initialized) return;
    redasm::state::initialized = false;
    rdcontext_destroy();
    redasm::pm::destroy();
}

void rd_setloglevel(RDLogLevel l) {
    spdlog::trace("rd_setloglevel('{}')", static_cast<int>(l));

    // clang-format off
    switch(l) {
        case LOGLEVEL_TRACE: spdlog::set_level(spdlog::level::trace); break;
        case LOGLEVEL_INFO: spdlog::set_level(spdlog::level::info); break;
        case LOGLEVEL_WARNING: spdlog::set_level(spdlog::level::warn); break;
        case LOGLEVEL_ERROR: spdlog::set_level(spdlog::level::err); break;
        case LOGLEVEL_CRITICAL: spdlog::set_level(spdlog::level::critical); break;
        case LOGLEVEL_OFF: spdlog::set_level(spdlog::level::off); break;
        default: spdlog::error("set_loglevel(): Invalid log-level"); break;
    }
    // clang-format on
}

void rd_addproblem(RDAddress address, const char* problem) {
    spdlog::trace("rd_addproblem({:x}, '{}')", address, problem);

    if(problem && redasm::state::context)
        redasm::state::context->add_problem(address, problem);
}

void rd_addref(RDAddress fromaddr, RDAddress toaddr, usize type) {
    spdlog::trace("rd_addref({:x}, {:x}, {})", fromaddr, toaddr, type);

    if(redasm::state::context)
        redasm::state::context->add_ref(fromaddr, toaddr, type);
}

usize rd_getrefsfrom(RDAddress fromaddr, const RDRef** refs) {
    spdlog::trace("rd_getrefsfrom({}, {})", fromaddr, fmt::ptr(refs));
    if(!redasm::state::context) return 0;

    static std::vector<RDRef> r;
    r = redasm::state::context->get_refs_from(fromaddr);
    if(refs) *refs = r.data();
    return r.size();
}

usize rd_getrefsfromtype(RDAddress fromaddr, usize type, const RDRef** refs) {
    spdlog::trace("rd_getrefsfromtype({}, {}, {})", fromaddr, type,
                  fmt::ptr(refs));
    if(!redasm::state::context) return 0;

    static std::vector<RDRef> r;
    r = redasm::state::context->get_refs_from_type(fromaddr, type);
    if(refs) *refs = r.data();
    return r.size();
}

usize rd_getrefsto(RDAddress toaddr, const RDRef** refs) {
    spdlog::trace("rd_getrefsto({}, {})", toaddr, fmt::ptr(refs));
    if(!redasm::state::context) return 0;

    static std::vector<RDRef> r;
    r = redasm::state::context->get_refs_to(toaddr);
    if(refs) *refs = r.data();
    return r.size();
}

usize rd_getrefstotype(RDAddress toaddr, usize type, const RDRef** refs) {
    spdlog::trace("rd_getrefstotype({}, {}, {})", toaddr, type, fmt::ptr(refs));
    if(!redasm::state::context) return 0;

    static std::vector<RDRef> r;
    r = redasm::state::context->get_refs_to_type(toaddr, type);
    if(refs) *refs = r.data();
    return r.size();
}

usize rd_getproblems(const RDProblem** problems) {
    spdlog::trace("rd_getproblems({})", fmt::ptr(problems));
    static std::vector<RDProblem> res;

    const redasm::Context* ctx = redasm::state::context;
    if(!ctx) return 0;

    res.reserve(ctx->problems.size());
    for(const auto& [address, problem] : ctx->problems)
        res.emplace_back(address, problem.c_str());

    if(problems) *problems = res.data();
    return res.size();
}

usize rd_test(RDBuffer* buffer, RDTestResult** result) {
    spdlog::trace("rd_test({}, {})", fmt::ptr(buffer), fmt::ptr(result));

    static std::vector<RDTestResult> res;

    redasm::state::context = nullptr; // Deselect active context
    redasm::state::contextlist.clear();
    res.clear();

    RDLoaderRequest req = {
        .file = buffer,
        .path = buffer->source,
        .name = redasm::utils::get_filename(buffer->source).data(), // NOLINT
        .ext = redasm::utils::get_ext(buffer->source).data(),       // NOLINT
    };

    foreach_loaders(lp, {
        if(!lp->accept || !lp->accept(lp, &req)) continue;

        auto* ctx = new redasm::Context(buffer);
        redasm::state::context = ctx; // Set context as active

        if(ctx->try_load(lp)) {
            redasm::state::contextlist.push_back(ctx);

            res.emplace_back(RDTestResult{
                ctx->loaderplugin,
                ctx->processorplugin,
                redasm::api::to_c(ctx),
            });
        }
        else {
            redasm::state::context = nullptr;
            delete ctx;
        }
    });

    // Sort results by priority
    std::ranges::stable_partition(res, std::not_fn([](const RDTestResult& x) {
                                      return x.loaderplugin->flags & PF_LAST;
                                  }));

    redasm::state::context = nullptr; // Deselect "test" context
    if(result) *result = res.data();
    return res.size();
}

void rd_disassemble() {
    spdlog::trace("rd_disassemble()");
    if(!redasm::state::context) return;

    while(rd_tick(nullptr))
        ;
}

void rd_select(const RDTestResult* tr) {
    spdlog::trace("rd_select({})", fmt::ptr(tr));

    if(tr) {
        redasm::state::context = redasm::api::from_c(tr->context);
        redasm::state::context->setup(tr->processorplugin);
    }
    else
        redasm::state::context = nullptr;

    // Discard other contexts
    while(!redasm::state::contextlist.empty()) {
        redasm::Context* c = redasm::state::contextlist.back();
        redasm::state::contextlist.pop_back();
        if(c != redasm::state::context) delete c;
    }
}

bool rdcontext_destroy() {
    spdlog::trace("rdcontext_destroy()");
    if(!redasm::state::context) return false;

    delete redasm::state::context;
    redasm::state::context = nullptr;
    return true;
}

void rd_discard() {
    spdlog::trace("rd_discard()");
    for(redasm::Context* c : redasm::state::contextlist)
        delete c;
    redasm::state::contextlist.clear();
}

void rd_addsearchpath(const char* sp) {
    spdlog::trace("rd_addsearchpath('{}')", sp);
    if(sp) redasm::mm::add_searchpath(sp);
}

usize rd_getsearchpaths(const char*** spaths) {
    spdlog::trace("rd_getsearchpaths({})", fmt::ptr(spaths));

    static std::vector<const char*> res;

    const auto& sp = redasm::mm::get_searchpaths();
    res.resize(sp.size());

    std::ranges::transform(sp, res.begin(),
                           [](const std::string& x) { return x.c_str(); });

    if(spaths) *spaths = res.data();

    return res.size();
}

void rd_setuserdata(const char* k, uptr v) {
    spdlog::trace("rd_setuserdata('{}', {})", k, v);
    if(k && redasm::state::context) redasm::state::context->set_userdata(k, v);
}

bool rd_getuserdata(const char* k, uptr* v) {
    spdlog::trace("rd_getuserdata('{}', {})", k, fmt::ptr(v));
    if(!k) return false;

    auto ud = redasm::state::context->get_userdata(k);
    ud.map([&](uptr x) {
        if(v) *v = x;
    });

    return ud.has_value();
}

usize rd_getentries(RDAddress** entries) {
    spdlog::trace("rd_getentries({})", fmt::ptr(entries));
    if(!redasm::state::context) return {};

    static std::vector<RDAddress> res;

    res.clear();
    res.reserve(redasm::state::context->entrypoints.size());

    for(RDAddress ep : redasm::state::context->entrypoints)
        res.push_back(ep);

    if(entries) *entries = res.data();
    return res.size();
}

bool rd_getaddress(const char* name, RDAddress* address) {
    spdlog::trace("rd_getaddress('{}', {})", name, fmt::ptr(address));
    if(!name || !redasm::state::context) return false;

    auto addr = redasm::state::context->get_address(name);
    addr.map([&](RDAddress x) {
        if(address) *address = x;
    });
    return addr.has_value();
}

const char* rd_getname(RDAddress address) {
    spdlog::trace("rd_getname({:x})", address);
    if(!redasm::state::context) return nullptr;
    static std::string res;
    res = redasm::state::context->get_name(address);
    return res.empty() ? nullptr : res.c_str();
}

const char* rd_getcomment(RDAddress address) {
    spdlog::trace("rd_getcomment({:x})", address);
    static std::string res;
    res = redasm::state::context->get_comment(address);
    return res.empty() ? nullptr : res.c_str();
}

bool rd_gettype(RDAddress address, const RDType* t, RDValue* v) {
    spdlog::trace("rd_gettype({:x}, {}, {})", address, fmt::ptr(t),
                  fmt::ptr(v));
    if(!redasm::state::context || !t) return false;

    const RDSegment* seg =
        redasm::state::context->program.find_segment(address);

    if(seg) {
        return redasm::memory::get_type(seg, address, *t)
            .map_or(
                [&](const RDValue& x) {
                    *v = x;
                    return true;
                },
                false);
    }

    return false;
}

bool rd_gettypename(RDAddress address, const char* tname, RDValue* v) {
    spdlog::trace("rd_gettypename({:x}, '{}', {})", address, tname,
                  fmt::ptr(v));
    if(!redasm::state::context || !tname) return false;

    const RDSegment* seg =
        redasm::state::context->program.find_segment(address);

    if(seg) {
        return redasm::memory::get_type(seg, address, tname)
            .map_or(
                [&](const RDValue& x) {
                    *v = x;
                    return true;
                },
                false);
    }

    return false;
}

usize rd_getsegments(const RDSegment** segments) {
    spdlog::trace("rd_getsegments({})", fmt::ptr(segments));
    if(!redasm::state::context) return 0;
    if(segments) *segments = redasm::state::context->program.segments.data();
    return redasm::state::context->program.segments.size();
}

bool rd_setcomment(RDAddress address, const char* comment) {
    spdlog::trace("rd_setcomment({:x}, '{}')", address, comment);

    return redasm::state::context &&
           redasm::state::context->set_comment(address, comment);
}

bool rd_settype(RDAddress address, const RDType* type, RDValue* v) {
    return rd_settype_ex(address, type, 0, v);
}

bool rd_settype_ex(RDAddress address, const RDType* type, usize flags,
                   RDValue* v) {
    spdlog::trace("rd_settype_ex({:x}, {}, {})", address, fmt::ptr(type), flags,
                  fmt::ptr(v));

    if(!redasm::state::context || !type) return false;

    const RDSegment* seg =
        redasm::state::context->program.find_segment(address);
    if(!seg) return false;

    if(redasm::state::context->set_type(address, *type, flags)) {
        auto res = redasm::memory::get_type(seg, address, *type);

        res.map([&](RDValue& x) {
            if(v)
                *v = x;
            else
                rdvalue_destroy(&x);
        });

        return res.has_value();
    }

    return false;
}

bool rd_settypename(RDAddress address, const char* tname, RDValue* v) {
    return rd_settypename_ex(address, tname, 0, v);
}

bool rd_settypename_ex(RDAddress address, const char* tname, usize flags,
                       RDValue* v) {
    spdlog::trace("rd_settypename_ex({:x}, '{}', {}, {})", address, tname,
                  flags, fmt::ptr(v));
    if(!redasm::state::context || !tname) return false;

    const RDSegment* seg =
        redasm::state::context->program.find_segment(address);
    if(!seg) return false;

    if(redasm::state::context->set_type(address, tname, flags)) {
        auto res = redasm::memory::get_type(seg, address, tname);

        res.map([&](RDValue& x) {
            if(v)
                *v = x;
            else
                rdvalue_destroy(&x);
        });

        return res.has_value();
    }

    return false;
}

bool rd_setfunction(RDAddress address) { return rd_setfunction_ex(address, 0); }

bool rd_setfunction_ex(RDAddress address, usize flags) {
    spdlog::trace("rd_setfunction_ex({:x}, {})", address, flags);

    return redasm::state::context &&
           redasm::state::context->set_function(address, flags);
}

bool rd_setentry(RDAddress address, const char* name) {
    spdlog::trace("rd_setentry({:x}, '{}')", address, name);
    std::string n;
    if(name) n = name;

    return redasm::state::context &&
           redasm::state::context->set_entry(address, n);
}

bool rd_setname(RDAddress address, const char* name) {
    return rd_setname_ex(address, name, 0);
}

bool rd_setname_ex(RDAddress address, const char* name, usize flags) {
    spdlog::trace("rd_setname_ex({:x}, '{}', {:x})", address, name, flags);
    return name && redasm::state::context &&
           redasm::state::context->set_name(address, name, flags);
}

bool rd_tick(const RDWorkerStatus** s) {
    spdlog::trace("rd_tick({})", fmt::ptr(s));
    if(redasm::state::context) return redasm::state::context->worker.execute(s);
    return false;
}

RDBuffer* rd_getfile() {
    spdlog::trace("rd_getfile()");
    if(!redasm::state::context) return nullptr;
    return redasm::state::context->program.file;
}

const char* rd_rendertext(RDAddress address) {
    spdlog::trace("rd_rendertext({:x})", address);
    if(!redasm::state::context) return nullptr;
    static std::string s;

    auto it = redasm::state::context->listing.lower_bound(address);

    // Find first code item
    while(it != redasm::state::context->listing.end() &&
          it->address == address) {
        if(it->type == LISTINGITEM_INSTRUCTION) break;
        it++;
    }

    if(it == redasm::state::context->listing.end() || it->address != address)
        return "";

    LIndex lidx = std::distance(redasm::state::context->listing.cbegin(), it);

    redasm::Surface sf{SURFACE_TEXT};
    sf.seek(lidx);
    sf.render(1);

    s = sf.get_text();
    return s.c_str();
}

bool rd_tooffset(RDAddress address, RDOffset* offset) {
    spdlog::trace("rd_tooffset({:x}, {})", address, fmt::ptr(offset));
    if(!redasm::state::context) return false;

    auto off = redasm::state::context->program.to_offset(address);
    off.map([&](RDOffset x) {
        if(offset) *offset = x;
    });
    return off.has_value();
}

bool rd_toaddress(RDOffset offset, RDAddress* address) {
    spdlog::trace("rd_address({:x}, {})", offset, fmt::ptr(address));
    if(!redasm::state::context) return false;

    auto addr = redasm::state::context->program.to_address(offset);
    addr.map([&](RDOffset x) {
        if(address) *address = x;
    });
    return addr.has_value();
}

const RDSegment* rd_findsegment(RDAddress address) {
    spdlog::trace("rd_findsegment({:x})", address);
    if(!redasm::state::context) return nullptr;
    return redasm::state::context->program.find_segment(address);
}

void rd_log(const char* s) {
    spdlog::trace("rd_log('{}')", s);
    if(s) redasm::state::log(s);
}

void rd_status(const char* s) {
    spdlog::trace("rd_status('{}')", s);
    if(s) redasm::state::status(s);
}

const char* rd_symbolize(const char* s) {
    spdlog::trace("rd_symbolize('{}')", s);
    if(!s) return nullptr;

    static std::string res;
    res = s;

    for(char& ch : res) {
        if(std::isalnum(ch) || ch == '_') continue;
        ch = '_';
    }

    return res.c_str();
}
