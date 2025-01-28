#include "../context.h"
#include "../memory/file.h"
#include "../plugins/modulemanager.h"
#include "../plugins/pluginmanager.h"
#include "../state.h"
#include "../surface/surface.h"
#include "../utils/utils.h"
#include "marshal.h"
#include <algorithm>
#include <cctype>
#include <fstream>
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

void rd_destroy(void* obj) {
    spdlog::trace("rd_destroy({})", fmt::ptr(obj));
    delete reinterpret_cast<redasm::Object*>(obj);
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

    if(problem && redasm::state::context) {
        redasm::state::context->add_problem(
            address - redasm::state::context->baseaddress, problem);
    }
}

void rd_addref(RDAddress fromaddr, RDAddress toaddr, usize type) {
    spdlog::trace("rd_addref({:x}, {:x}, {})", fromaddr, toaddr, type);

    if(redasm::state::context) {
        auto fromidx = redasm::state::context->address_to_index(fromaddr);
        auto toidx = redasm::state::context->address_to_index(toaddr);

        if(fromidx && toidx)
            redasm::state::context->add_ref(*fromidx, *toidx, type);
    }
}

usize rd_getrefsfrom(RDAddress fromaddr, const RDRef** refs) {
    spdlog::trace("rd_getrefsfrom({}, {})", fromaddr, fmt::ptr(refs));
    if(!redasm::state::context) return 0;

    static std::vector<RDRef> r;

    r = redasm::state::context->address_to_index(fromaddr).map_or(
        [](MIndex x) {
            return redasm::api::to_c(redasm::state::context->get_refs_from(x));
        },
        std::vector<RDRef>{});

    if(refs) *refs = r.data();
    return r.size();
}

usize rd_getrefsfromtype(RDAddress fromaddr, usize type, const RDRef** refs) {
    spdlog::trace("rd_getrefsfromtype({}, {}, {})", fromaddr, type,
                  fmt::ptr(refs));
    if(!redasm::state::context) return 0;

    static std::vector<RDRef> r;

    r = redasm::state::context->address_to_index(fromaddr).map_or(
        [type](MIndex x) {
            return redasm::api::to_c(
                redasm::state::context->get_refs_from_type(x, type));
        },
        std::vector<RDRef>{});

    if(refs) *refs = r.data();
    return r.size();
}

usize rd_getrefsto(RDAddress toaddr, const RDRef** refs) {
    spdlog::trace("rd_getrefsto({}, {})", toaddr, fmt::ptr(refs));
    if(!redasm::state::context) return 0;

    static std::vector<RDRef> r;

    r = redasm::state::context->address_to_index(toaddr).map_or(
        [](MIndex x) {
            return redasm::api::to_c(redasm::state::context->get_refs_to(x));
        },
        std::vector<RDRef>{});

    if(refs) *refs = r.data();
    return r.size();
}

usize rd_getrefstotype(RDAddress toaddr, usize type, const RDRef** refs) {
    spdlog::trace("rd_getrefstotype({}, {}, {})", toaddr, type, fmt::ptr(refs));
    if(!redasm::state::context) return 0;

    static std::vector<RDRef> r;

    r = redasm::state::context->address_to_index(toaddr).map_or(
        [type](MIndex x) {
            return redasm::api::to_c(
                redasm::state::context->get_refs_to_type(x, type));
        },
        std::vector<RDRef>{});

    if(refs) *refs = r.data();
    return r.size();
}

bool rd_addresstosegment(RDAddress address, RDSegment* res) {
    spdlog::trace("rd_addresstosegment({:x}, {})", address, fmt::ptr(res));

    const redasm::Context* ctx = redasm::state::context;
    if(!ctx) return false;

    MIndex idx = address - redasm::state::context->baseaddress;
    if(idx >= ctx->program.memory->size()) return false;

    return std::any_of(ctx->program.segments.begin(),
                       ctx->program.segments.end(),
                       [&](const redasm::Segment& s) {
                           if(idx >= s.index && idx < s.endindex) {
                               if(res) *res = redasm::api::to_c(s);
                               return true;
                           }

                           return false;
                       });
}

bool rd_offsettosegment(RDOffset offset, RDSegment* res) {
    spdlog::trace("rd_offsettosegment({:x}, {})", offset, fmt::ptr(res));

    const redasm::Context* ctx = redasm::state::context;
    if(!ctx) return false;

    return std::any_of(ctx->program.segments.begin(),
                       ctx->program.segments.end(),
                       [&](const redasm::Segment& s) {
                           if(offset >= s.offset && offset < s.endoffset) {
                               if(res) *res = redasm::api::to_c(s);
                               return true;
                           }

                           return false;
                       });
}

bool rd_addresstooffset(RDAddress address, RDOffset* offset) {
    spdlog::trace("rd_addresstooffset({:x}, {})", address, fmt::ptr(offset));
    if(!redasm::state::context) return false;

    MIndex idx = address - redasm::state::context->baseaddress;

    if(idx < redasm::state::context->program.memory->size()) {
        auto off = redasm::state::context->index_to_offset(idx);
        off.map([&](RDOffset x) {
            if(offset) *offset = x;
        });
        return off.has_value();
    }

    return false;
}

bool rd_addresstoindex(RDAddress address, MIndex* index) {
    spdlog::trace("rd_addresstoindex({:x}, {})", address, fmt::ptr(index));
    if(!redasm::state::context) return false;

    auto v = redasm::state::context->address_to_index(address);
    v.map([&](MIndex x) {
        if(index) *index = x;
    });
    return v.has_value();
}

bool rd_offsettoaddress(RDOffset offset, RDAddress* address) {
    spdlog::trace("rd_offsettoaddress({:x}, {})", offset, fmt::ptr(address));
    if(!redasm::state::context) return false;
    if(offset >= redasm::state::context->program.file->size()) return false;

    RDAddress addr = redasm::state::context->baseaddress + offset;

    for(const redasm::Segment& s : redasm::state::context->program.segments) {
        if(offset >= s.offset && offset < s.endoffset) {
            addr = redasm::state::context->baseaddress + s.index +
                   (offset - s.offset);
            break;
        }
    }

    if(address) *address = addr;
    return true;
}

usize rd_getproblems(const RDProblem** problems) {
    spdlog::trace("rd_getproblems({})", fmt::ptr(problems));
    static std::vector<RDProblem> res;

    const redasm::Context* ctx = redasm::state::context;
    if(!ctx) return 0;

    res.reserve(ctx->problems.size());
    for(const auto& [index, problem] : ctx->problems)
        res.emplace_back(ctx->baseaddress + index, problem.c_str());

    if(problems) *problems = res.data();
    return res.size();
}

RDBuffer* rd_loadfile(const char* filepath) {
    spdlog::trace("rd_loadfile('{}')", filepath);
    if(!filepath) return nullptr;

    std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);

    if(!ifs.is_open()) {
        spdlog::error("Cannot open '{}'", filepath);
        return nullptr;
    }

    auto* b = new redasm::File(filepath);
    b->resize(ifs.tellg());
    ifs.seekg(0);
    ifs.read(reinterpret_cast<char*>(b->data()), b->size());
    return redasm::api::to_c(b);
}

usize rd_test(RDBuffer* buffer, RDTestResult** result) {
    spdlog::trace("rd_test({}, {})", fmt::ptr(buffer), fmt::ptr(result));

    static std::vector<RDTestResult> res;
    std::shared_ptr<redasm::AbstractBuffer> b{redasm::api::from_c(buffer)};

    redasm::state::context = nullptr; // Deselect active context
    redasm::state::contextlist.clear();
    res.clear();

    RDLoaderRequest req = {
        .path = b->source.c_str(),
        .name = redasm::utils::get_filename(b->source).data(), // NOLINT
        .ext = redasm::utils::get_ext(b->source).data(),       // NOLINT
        .file = redasm::api::to_c(b.get()),
    };

    foreach_loaders(lp, {
        if(!lp->accept || !lp->accept(lp, &req)) continue;

        auto* ctx = new redasm::Context(b);
        redasm::state::context = ctx; // Set context as active

        if(ctx->try_load(lp)) {
            redasm::state::contextlist.push_back(ctx);

            res.emplace_back(RDTestResult{
                ctx->loaderplugin,
                ctx->processorplugin,
                redasm::api::to_c(ctx),
            });
        }
        else
            delete ctx;
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

    for(MIndex ep : redasm::state::context->entrypoints) {
        auto v = redasm::state::context->index_to_address(ep);
        assume(v.has_value());
        res.push_back(*v);
    }

    if(entries) *entries = res.data();
    return res.size();
}

bool rd_getaddress(const char* name, RDAddress* address) {
    spdlog::trace("rd_getaddress('{}', {})", name, fmt::ptr(address));
    if(!name || !redasm::state::context) return false;

    auto addr =
        redasm::state::context->get_index(name).and_then([](MIndex idx) {
            return redasm::state::context->index_to_address(idx);
        });

    addr.map([&](RDAddress x) {
        if(address) *address = x;
    });
    return addr.has_value();
}

const char* rd_getname(RDAddress address) {
    spdlog::trace("rd_getname({:x})", address);
    if(!redasm::state::context) return nullptr;
    static std::string res;

    if(auto idx = redasm::state::context->address_to_index(address); idx)
        res = redasm::state::context->get_name(*idx);

    return res.empty() ? nullptr : res.c_str();
}

const char* rd_getcomment(RDAddress address) {
    spdlog::trace("rd_getcomment({:x})", address);
    static std::string res;

    if(auto idx = redasm::state::context->address_to_index(address); idx)
        res = redasm::state::context->get_comment(*idx);

    return res.empty() ? nullptr : res.c_str();
}

bool rd_gettype(RDAddress address, const RDType* t, RDValue* v) {
    spdlog::trace("rd_gettype({:x}, {}, {})", address, fmt::ptr(t),
                  fmt::ptr(v));
    if(!redasm::state::context || !t) return false;

    if(auto idx = redasm::state::context->address_to_index(address); idx) {
        return redasm::state::context->program.memory->get_type(*idx, *t)
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

    if(auto idx = redasm::state::context->address_to_index(address); idx) {
        return redasm::state::context->program.memory->get_type(*idx, tname)
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
    static std::vector<RDSegment> res;

    if(segments) {
        const redasm::Context* ctx = redasm::state::context;
        res.resize(ctx->program.segments.size());

        for(usize i = 0; i < ctx->program.segments.size(); i++)
            res[i] = redasm::api::to_c(ctx->program.segments[i]);

        *segments = res.data();
    }

    return redasm::state::context->program.segments.size();
}

bool rd_mapsegment(const char* name, RDAddress address, RDAddress endaddress,
                   RDOffset offset, RDOffset endoffset, u8 perm) {
    spdlog::trace("rd_mapsegment('{}', {:x}, {:x}, {:x}, {:x}, {:x})", name,
                  address, endaddress, offset, endoffset, perm);

    if(redasm::state::context && name) {
        MIndex startidx = address - redasm::state::context->baseaddress;
        MIndex endidx = endaddress - redasm::state::context->baseaddress;
        redasm::state::context->map_segment(name, startidx, endidx, offset,
                                            endoffset, perm);
    }

    return true;
}

bool rd_mapsegment_n(const char* name, RDAddress address, usize asize,
                     RDOffset offset, usize osize, u8 perm) {
    spdlog::trace("rd_mapsegment_n('{}', {:x}, {:x}, {:x}, {:x}, {:x})", name,
                  address, asize, offset, osize, perm);

    if(redasm::state::context && name) {
        MIndex startidx = address - redasm::state::context->baseaddress;
        MIndex endidx = (address + asize) - redasm::state::context->baseaddress;
        redasm::state::context->map_segment(name, startidx, endidx, offset,
                                            offset + osize, perm);
    }

    return false;
}

bool rd_setcomment(RDAddress address, const char* comment) {
    spdlog::trace("rd_setcomment({:x}, '{}')", address, comment);

    return redasm::state::context->address_to_index(address).map_or(
        [&](MIndex idx) {
            return redasm::state::context->set_comment(idx, comment);
        },
        false);
}

bool rd_settype(RDAddress address, const RDType* type, RDValue* v) {
    return rd_settype_ex(address, type, 0, v);
}

bool rd_settype_ex(RDAddress address, const RDType* type, usize flags,
                   RDValue* v) {
    spdlog::trace("rd_settype_ex({:x}, {}, {})", address, fmt::ptr(type), flags,
                  fmt::ptr(v));

    if(redasm::state::context && type) {
        auto idx = redasm::state::context->address_to_index(address);
        if(idx && redasm::state::context->set_type(*idx, *type, flags)) {
            auto res =
                redasm::state::context->program.memory->get_type(*idx, *type);

            res.map([&](RDValue& x) {
                if(v)
                    *v = x;
                else
                    rdvalue_destroy(&x);
            });

            return res.has_value();
        }
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

    if(redasm::state::context && tname) {
        auto idx = redasm::state::context->address_to_index(address);

        if(idx && redasm::state::context->set_type(*idx, tname, flags)) {
            auto res =
                redasm::state::context->program.memory->get_type(*idx, tname);

            res.map([&](RDValue& x) {
                if(v)
                    *v = x;
                else
                    rdvalue_destroy(&x);
            });

            return res.has_value();
        }
    }

    return false;
}

bool rd_maptype(RDOffset offset, RDAddress address, const RDType* t) {
    return rd_maptype_ex(offset, address, t, 0);
}

bool rd_maptypename(RDOffset offset, RDAddress address, const char* tname) {
    return rd_maptypename_ex(offset, address, tname, 0);
}

bool rd_maptype_ex(RDOffset offset, RDAddress address, const RDType* t,
                   usize flags) {
    spdlog::trace("rd_maptype_ex({:x}, {:x}, {}, {})", offset, address,
                  fmt::ptr(t), flags);

    if(redasm::state::context && t) {
        auto idx = redasm::state::context->address_to_index(address);
        if(!idx) return false;

        usize sz = redasm::state::context->types.size_of(*t);
        redasm::state::context->memory_copy(*idx, offset, offset + sz);

        if(!redasm::state::context->set_type(*idx, *t, 0)) return false;
        auto v = redasm::state::context->program.file->get_type(*idx, *t);
        v.map([](RDValue& x) { rdvalue_destroy(&x); });
        return v.has_value();
    }

    return false;
}

bool rd_maptypename_ex(RDOffset offset, RDAddress address, const char* tname,
                       usize flags) {
    spdlog::trace("rd_maptypename_ex({:x}, {:x}, '{}', {})", offset, address,
                  tname, flags);
    if(redasm::state::context && tname) {
        auto idx = redasm::state::context->address_to_index(address);
        if(!idx) return false;

        usize sz = redasm::state::context->types.size_of(tname);
        redasm::state::context->memory_copy(*idx, offset, offset + sz);

        if(!redasm::state::context->set_type(*idx, tname, 0)) return false;
        auto v = redasm::state::context->program.file->get_type(*idx, tname);
        v.map([](RDValue& x) { rdvalue_destroy(&x); });
        return v.has_value();
    }

    return false;
}

bool rd_setfunction(RDAddress address) { return rd_setfunction_ex(address, 0); }

bool rd_setfunction_ex(RDAddress address, usize flags) {
    spdlog::trace("rd_setfunction_ex({:x}, {})", address, flags);

    return redasm::state::context->address_to_index(address).map_or(
        [&](MIndex idx) {
            return redasm::state::context->set_function(idx, flags);
        },
        false);
}

bool rd_setentry(RDAddress address, const char* name) {
    spdlog::trace("rd_setentry({:x}, '{}')", address, name);
    std::string n;
    if(name) n = name;

    return redasm::state::context->address_to_index(address).map_or(
        [&](MIndex idx) { return redasm::state::context->set_entry(idx, n); },
        false);
}

bool rd_setname(RDAddress address, const char* name) {
    return rd_setname_ex(address, name, 0);
}

bool rd_setname_ex(RDAddress address, const char* name, usize flags) {
    spdlog::trace("rd_setname_ex({:x}, '{}', {:x})", address, name, flags);
    if(!name || !redasm::state::context) return false;

    return redasm::state::context->address_to_index(address).map_or(
        [&](MIndex idx) {
            return redasm::state::context->set_name(idx, name, flags);
        },
        false);
}

bool rd_tick(const RDWorkerStatus** s) {
    spdlog::trace("rd_tick({})", fmt::ptr(s));
    if(redasm::state::context) return redasm::state::context->worker.execute(s);
    return false;
}

usize rd_getmemory(const RDByte** data) {
    spdlog::trace("rd_getmemory({})", fmt::ptr(data));

    if(!redasm::state::context) return 0;
    const auto& m = redasm::state::context->program.memory;
    if(!m) return 0;

    if(data) *data = redasm::api::to_c(m->data());
    return m->size();
}

bool rd_getbyte(usize idx, RDByte* b) {
    spdlog::trace("rd_getbyte({}, {})", idx, fmt::ptr(b));
    if(!redasm::state::context) return false;

    const auto& mem = redasm::state::context->program.memory;
    if(idx >= mem->size()) return false;
    if(b) *b = redasm::api::to_c(mem->at(idx));
    return true;
}

usize rd_getfile(const u8** data) {
    spdlog::trace("rd_getfile({})", fmt::ptr(data));

    if(!redasm::state::context) return 0;

    const auto& f = redasm::state::context->program.file;
    if(!f) return 0;

    // HACK: static_cast<> shouldn't be used
    if(data) *data = static_cast<const redasm::File*>(f.get())->data();
    return f->size();
}

const char* rd_rendertext(RDAddress address) {
    spdlog::trace("rd_rendertext({:x})", address);
    if(!redasm::state::context) return nullptr;
    static std::string s;

    auto idx = redasm::state::context->address_to_index(address);
    if(!idx) return "";

    auto it = redasm::state::context->listing.lower_bound(*idx);

    // Find first code item
    while(it != redasm::state::context->listing.end() && it->index == *idx) {
        if(it->type == LISTINGITEM_INSTRUCTION) break;
        it++;
    }

    if(it == redasm::state::context->listing.end() || it->index != *idx)
        return "";

    LIndex lidx = std::distance(redasm::state::context->listing.cbegin(), it);

    redasm::Surface sf{SURFACE_TEXT};
    sf.seek(lidx);
    sf.render(1);

    s = sf.get_text();
    return s.c_str();
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
