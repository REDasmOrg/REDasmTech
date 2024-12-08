#include "redasm.h"
#include "../../builtins/analyzer.h"
#include "../../builtins/loader.h"
#include "../../builtins/processor.h"
#include "../../context.h"
#include "../../memory/file.h"
#include "../../modulemanager.h"
#include "../../state.h"
#include "../../surface/surface.h"
#include "../marshal.h"
#include "../python/module.h"
#include "buffer.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <ranges>
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

namespace {

bool initialized = false;

}

bool is_alnum(u8 b) { return std::isalnum(b); }
bool is_alpha(u8 b) { return std::isalpha(b); }
bool is_lower(u8 b) { return std::islower(b); }
bool is_upper(u8 b) { return std::isupper(b); }
bool is_digit(u8 b) { return std::isdigit(b); }
bool is_xdigit(u8 b) { return std::isxdigit(b); }
bool is_cntrl(u8 b) { return std::iscntrl(b); }
bool is_graph(u8 b) { return std::isgraph(b); }
bool is_space(u8 b) { return std::isspace(b); }
bool is_blank(u8 b) { return std::isblank(b); }
bool is_print(u8 b) { return std::isprint(b); }
bool is_punct(u8 b) { return std::ispunct(b); }

bool init(const RDInitParams* params) {
    spdlog::trace("init({})", fmt::ptr(params));

    if(initialized)
        return true;

    initialized = true;

    // clang-format off
    state::params = {
        .onlog    = [](const char* arg, void*) { spdlog::info("LOG: {}", arg); },
        .onstatus = [](const char* arg, void*) { spdlog::info("STATUS: {}", arg); },
        .onerror  = [](const char* arg, void*) { spdlog::error("ERROR: {}", arg); },
        .userdata = nullptr,
    };

    if(params) {
        if(params->onlog) state::params.onlog = params->onlog;
        if(params->onstatus) state::params.onstatus = params->onstatus;
        if(params->onerror) state::params.onerror = params->onerror;
        state::params.userdata = params->userdata;
    }
    // clang-format on

    builtins::register_loaders();
    builtins::register_processors();
    builtins::register_analyzers();
    redasm::load_modules();

    if(python::init()) {
        python::main();

        // Keep 'Binary' loader as last option
        std::ranges::reverse(state::loaders);
        return true;
    }

    return false;
}

void deinit() {
    spdlog::trace("deinit()");

    if(!initialized)
        return;

    initialized = false;

    for(RDAnalyzer& a : state::analyzers) {
        if(a.free)
            a.free(&a);
    }

    state::analyzers.clear();

    for(RDProcessor& p : state::processors) {
        if(p.free)
            p.free(&p);
    }

    state::processors.clear();

    for(RDLoader& l : state::loaders) {
        if(l.free)
            l.free(&l);
    }

    state::loaders.clear();

    redasm::unload_modules();
    python::deinit();
}

void set_loglevel(RDLogLevel l) {
    spdlog::trace("set_loglevel('{}')", static_cast<int>(l));

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

void add_searchpath(const std::string& sp) {
    spdlog::trace("add_searchpath('{}')", sp);
    redasm::add_searchpath(sp);
}

void set_userdata(const std::string& k, uptr v) {
    spdlog::trace("set_userdata('{}', {:x})", k, v);

    if(state::context)
        state::context->set_userdata(k, v);
}

tl::optional<uptr> get_userdata(const std::string& k) {
    spdlog::trace("get_userdata('{}')", k);

    if(state::context)
        return state::context->get_userdata(k);

    return tl::nullopt;
}

const SearchPaths& get_searchpaths() {
    spdlog::trace("get_searchpaths()");
    return redasm::get_searchpaths();
}

void log(std::string_view s) { state::log(s); }
void status(std::string_view s) { state::log(s); }

std::string symbolize(std::string s) {
    for(char& ch : s) {
        if(std::isalnum(ch) || ch == '_')
            continue;

        ch = '_';
    }

    return s;
}

RDBuffer* load_file(const std::string& filepath) {
    spdlog::trace("load_file('{}')", filepath);

    std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);

    if(!ifs.is_open()) {
        spdlog::error("Cannot open '{}'", filepath);
        return nullptr;
    }

    auto* b = new File(filepath);
    b->resize(ifs.tellg());
    ifs.seekg(0);
    ifs.read(reinterpret_cast<char*>(b->data()), b->size());
    return api::to_c(b);
}

std::vector<RDTestResult> test(RDBuffer* buffer) {
    spdlog::trace("test({})", fmt::ptr(buffer));

    state::context = nullptr; // Deselect active context
    state::contextlist.clear();

    std::vector<RDTestResult> res;
    std::shared_ptr<AbstractBuffer> b{api::from_c(buffer)};

    for(RDLoader& ldr : state::loaders) {
        auto* ctx = new Context(b, &ldr);

        if(ctx->activate()) {
            state::contextlist.push_back(ctx);

            res.emplace_back(RDTestResult{
                ctx->loader,
                ctx->processor,
                api::to_c(ctx),
            });
        }
        else
            delete ctx;
    }

    state::context = nullptr; // Deselect "test" context
    return res;
}

void select(RDContext* context) {
    spdlog::trace("select({})", fmt::ptr(context));

    Context* ctx = api::from_c(context);

    state::context = ctx;
    assume(state::context);

    // Discard other contexts
    while(!state::contextlist.empty()) {
        Context* c = state::contextlist.back();
        state::contextlist.pop_back();
        if(c != ctx)
            delete c;
    }
}

void free(Object* obj) { delete obj; }

bool destroy() {
    spdlog::trace("destroy()");
    if(!state::context)
        return false;

    delete state::context;
    state::context = nullptr;

    return true;
}

void discard() {
    spdlog::trace("discard()");

    for(Context* c : state::contextlist)
        delete c;

    state::contextlist.clear();
}

void disassemble() {
    spdlog::trace("disassemble()");

    if(!state::context)
        return;

    while(tick(nullptr))
        ;
}

bool tick(const RDAnalysisStatus** s) {
    spdlog::trace("tick({})", fmt::ptr(s));

    if(!state::context)
        return false;

    return state::context->disassembler.execute(s);
}

std::string to_hex_n(usize v, int n) {
    if(state::context)
        return state::context->to_hex(v, n);
    return {};
}

std::string to_hex(usize v) {
    if(state::context)
        return state::context->to_hex(v);
    return {};
}

std::vector<RDAddress> get_entries() {
    spdlog::trace("get_entries()");

    if(!state::context)
        return {};

    std::vector<RDAddress> entries;
    entries.reserve(state::context->entrypoints.size());

    for(MIndex ep : state::context->entrypoints) {
        auto v = state::context->index_to_address(ep);
        assume(v.has_value());
        entries.push_back(*v);
    }

    return entries;
}

std::vector<RDProblem> get_problems() {
    spdlog::trace("get_problems()");

    Context* ctx = state::context;

    if(!ctx)
        return {};

    std::vector<RDProblem> res;
    res.reserve(ctx->problems.size());

    for(const auto& [index, problem] : ctx->problems)
        res.emplace_back(ctx->baseaddress + index, problem.c_str());

    return res;
}

usize get_segments(const RDSegment** segments) {
    spdlog::trace("get_segments()");

    static std::vector<RDSegment> res;

    if(segments) {
        const Context* ctx = state::context;
        res.resize(ctx->segments.size());

        for(usize i = 0; i < ctx->segments.size(); i++)
            res[i] = api::to_c(ctx->segments[i]);

        *segments = res.data();
    }

    return state::context->segments.size();
}

bool find_segment(RDAddress address, RDSegment* segment) {
    spdlog::trace("find_segment()");

    auto idx = state::context->address_to_index(address);
    if(!idx)
        return false;

    const Context* ctx = state::context;

    for(const Segment& s : ctx->segments) {
        if(s.index < *idx || *idx >= s.endoffset)
            continue;

        if(segment)
            *segment = api::to_c(s);

        return true;
    }

    return false;
}

std::vector<RDRef> get_refsfrom(RDAddress fromaddr) {
    spdlog::trace("get_refsfrom({})", fromaddr);
    if(!state::context)
        return {};

    return state::context->address_to_index(fromaddr).map_or(
        [](MIndex x) { return api::to_c(state::context->get_refs_from(x)); },
        std::vector<RDRef>{});
}

std::vector<RDRef> get_refsfromtype(RDAddress fromaddr, usize type) {
    spdlog::trace("get_refsfromtype({}, {})", fromaddr, type);
    if(!state::context)
        return {};

    return state::context->address_to_index(fromaddr).map_or(
        [type](MIndex x) {
            return api::to_c(state::context->get_refs_from_type(x, type));
        },
        std::vector<RDRef>{});
}

std::vector<RDRef> get_refsto(RDAddress toaddr) {
    spdlog::trace("get_refsto({})", toaddr);
    if(!state::context)
        return {};

    return state::context->address_to_index(toaddr).map_or(
        [](MIndex x) { return api::to_c(state::context->get_refs_to(x)); },
        std::vector<RDRef>{});
}

std::vector<RDRef> get_refstotype(RDAddress toaddr, usize type) {
    spdlog::trace("get_refstotype({}, {})", toaddr, type);
    if(!state::context)
        return {};

    return state::context->address_to_index(toaddr).map_or(
        [type](MIndex x) {
            return api::to_c(state::context->get_refs_to_type(x, type));
        },
        std::vector<RDRef>{});
}

usize get_bytes(const RDByte** bytes) {
    spdlog::trace("get_bytes({})", fmt::ptr(bytes));

    const auto& m = state::context->memory;

    if(!m)
        return 0;

    if(*bytes)
        *bytes = reinterpret_cast<const RDByte*>(m->data());

    return m->size();
}

void set_bits(int bits) {
    spdlog::trace("set_bits({})", bits);

    if(state::context)
        state::context->bits = bits;
}

int get_bits() {
    spdlog::trace("get_bits()");

    if(!state::context)
        return 0;

    return state::context->bits.value_or(0);
}

bool map_segment(const std::string& name, RDAddress address,
                 RDAddress endaddress, RDOffset offset, RDOffset endoffset,
                 usize type) {
    spdlog::trace("map_segment('{}', {:x}, {:x}, {:x}, {:x}, {:x})", name,
                  address, endaddress, offset, endoffset, type);

    auto start = state::context->address_to_index(address);
    if(!start) {
        spdlog::error("map_segment: start address {:x} out of bounds", address);
        return false;
    }

    auto end = state::context->address_to_index(endaddress);
    if(!end) {
        spdlog::error("map_segment: end address {:x} out of bounds",
                      endaddress);
        return false;
    }

    state::context->map_segment(name, *start, *end, offset, endoffset, type);
    return true;
}

bool map_segment_n(const std::string& name, RDAddress address, usize asize,
                   RDOffset offset, usize osize, usize type) {
    spdlog::trace("map_segment_n('{}', {:x}, {:x}, {:x}, {:x}, {:x})", name,
                  address, asize, offset, osize, type);

    auto start = state::context->address_to_index(address);
    if(!start) {
        spdlog::error("map_segment_n: start address {:x} out of bounds",
                      address);
        return false;
    }

    RDAddress endaddress = address + asize;

    auto end = state::context->address_to_index(endaddress);
    if(!end) {
        spdlog::error("map_segment_n: end address {:x} out of bounds", address);
        return false;
    }

    state::context->map_segment(name, *start, *end, offset, offset + osize,
                                type);
    return true;
}

bool set_name_ex(RDAddress address, const std::string& name, usize flags) {
    spdlog::trace("set_nameex({:x}, '{}', {:x})", address, name, flags);

    if(!state::context)
        return false;

    return state::context->address_to_index(address).map_or(
        [&](MIndex idx) { return state::context->set_name(idx, name, flags); },
        false);
}

tl::optional<RDAddress> get_address(std::string_view name) {
    if(!state::context)
        return tl::nullopt;

    return state::context->get_index(name).and_then(
        [](MIndex idx) { return state::context->index_to_address(idx); });
}

std::string get_comment(RDAddress address) {
    spdlog::trace("get_comment({:x})", address);

    if(auto idx = state::context->address_to_index(address); idx)
        return state::context->get_comment(*idx);

    return {};
}

std::string get_name(RDAddress address) {
    spdlog::trace("get_name({:x})", address);

    if(!state::context)
        return {};

    if(auto idx = state::context->address_to_index(address); idx)
        return state::context->get_name(*idx);

    return {};
}

std::string render_text(RDAddress address) {
    auto idx = state::context->address_to_index(address);
    if(!idx)
        return {};

    auto it = state::context->listing.lower_bound(*idx);

    // Find first code item
    while(it != state::context->listing.end() && it->index == *idx) {
        if(it->type == LISTINGITEM_INSTRUCTION)
            break;

        it++;
    }

    if(it == state::context->listing.end() || it->index != *idx)
        return {};

    LIndex lidx = std::distance(state::context->listing.cbegin(), it);

    Surface s{SURFACE_TEXT};
    s.seek(lidx);
    s.render(1);

    return std::string{s.get_text()};
}

tl::optional<typing::Value> set_type(RDAddress address, const RDType* t) {
    spdlog::trace("set_type({:x}, {})", address, fmt::ptr(t));

    if(t) {
        auto idx = state::context->address_to_index(address);
        if(idx && state::context->set_type(*idx, *t))
            return state::context->memory->get_type(*idx, *t);
    }

    return tl::nullopt;
}

tl::optional<typing::Value> set_typename(RDAddress address,
                                         typing::FullTypeName tname) {
    spdlog::trace("set_typename({:x}, '{}')", address, tname);

    auto idx = state::context->address_to_index(address);
    if(idx && state::context->set_type(*idx, tname))
        return state::context->memory->get_type(*idx, tname);
    return tl::nullopt;
}

bool set_comment(RDAddress address, std::string_view comment) {
    spdlog::trace("set_comment({:x}, {})", address, comment);

    return state::context->address_to_index(address).map_or(
        [&](MIndex idx) { return state::context->set_comment(idx, comment); },
        false);
}

bool set_function_ex(RDAddress address, usize flags) {
    spdlog::trace("set_function_ex({:x}, {})", address, flags);

    return state::context->address_to_index(address).map_or(
        [&](MIndex idx) { return state::context->set_function(idx, flags); },
        false);
}

bool set_entry(RDAddress address, const std::string& name) {
    spdlog::trace("set_entry({:x}, '{}')", address, name);

    return state::context->address_to_index(address).map_or(
        [&](MIndex idx) { return state::context->set_entry(idx, name); },
        false);
}

void add_ref(RDAddress fromaddr, RDAddress toaddr, usize type) {
    spdlog::trace("add_ref({:x}, {:x}, {})", fromaddr, toaddr, type);

    if(state::context) {
        auto fromidx = state::context->address_to_index(fromaddr);
        auto toidx = state::context->address_to_index(toaddr);

        if(fromidx && toidx)
            state::context->add_ref(*fromidx, *toidx, type);
    }
}

void add_problem(RDAddress address, const std::string& problem) {
    spdlog::trace("add_problem({:x}, '{}')", address, problem);

    if(state::context) {
        state::context->add_problem(address - state::context->baseaddress,
                                    problem);
    }
}

bool is_address(RDAddress address) {
    spdlog::trace("is_address({:x})", address);
    const Context* ctx = state::context;
    return ctx && ctx->is_address(address);
}

bool address_to_segment(RDAddress address, RDSegment* res) {
    spdlog::trace("address_to_segment({:x}, {})", address, fmt::ptr(res));
    usize idx = address - state::context->baseaddress;
    if(idx >= state::context->memory->size())
        return {};

    const Context* ctx = state::context;

    return std::any_of(ctx->segments.begin(), ctx->segments.end(),
                       [&](const Segment& s) {
                           if(idx >= s.index && idx < s.endindex) {
                               if(res)
                                   *res = api::to_c(s);
                               return true;
                           }

                           return false;
                       });
}

bool offset_to_segment(RDOffset offset, RDSegment* res) {
    spdlog::trace("offset_to_segment({:x}, {})", offset, fmt::ptr(res));
    if(offset >= state::context->file->size())
        return {};

    const Context* ctx = state::context;

    return std::any_of(ctx->segments.begin(), ctx->segments.end(),
                       [&](const Segment& s) {
                           if(offset >= s.offset && offset < s.endoffset) {
                               if(res)
                                   *res = api::to_c(s);
                               return true;
                           }

                           return false;
                       });
}

tl::optional<RDOffset> to_offset(RDAddress address) {
    spdlog::trace("to_offset({:x})", address);
    if(!state::context)
        return tl::nullopt;

    usize idx = address - state::context->baseaddress;
    if(idx < state::context->memory->size())
        return state::context->index_to_offset(idx);

    return tl::nullopt;
}

tl::optional<RDAddress> to_address(RDOffset offset) {
    spdlog::trace("to_address({:x})", offset);
    if(!state::context)
        return tl::nullopt;

    if(offset >= state::context->file->size())
        return {};

    RDAddress address = state::context->baseaddress + offset;

    for(const Segment& s : state::context->segments) {
        if(offset >= s.offset && offset < s.endoffset) {
            address =
                state::context->baseaddress + s.index + (offset - s.offset);
            break;
        }
    }

    return address;
}

tl::optional<RDAddress> from_reladdress(RDAddress reladdress) {
    spdlog::trace("from_reladdress({:x})", reladdress);
    if(!state::context)
        return tl::nullopt;

    RDAddress address = state::context->baseaddress + reladdress;

    if(address >= state::context->baseaddress &&
       address < state::context->end_baseaddress())
        return address;

    return tl::nullopt;
}

tl::optional<bool> get_bool(RDAddress address) {
    spdlog::trace("get_bool({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getbool(api::to_c(state::context->memory.get()),
                                        *idx);

    return tl::nullopt;
}

tl::optional<char> get_char(RDAddress address) {
    spdlog::trace("get_char({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getchar(api::to_c(state::context->memory.get()),
                                        *idx);

    return tl::nullopt;
}

tl::optional<u8> get_u8(RDAddress address) {
    spdlog::trace("get_u8({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getu8(api::to_c(state::context->memory.get()),
                                      *idx);

    return tl::nullopt;
}

tl::optional<u16> get_u16(RDAddress address) {
    spdlog::trace("get_u16({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getu16(api::to_c(state::context->memory.get()),
                                       *idx);

    return tl::nullopt;
}

tl::optional<u32> get_u32(RDAddress address) {
    spdlog::trace("get_u32({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getu32(api::to_c(state::context->memory.get()),
                                       *idx);

    return tl::nullopt;
}

tl::optional<u64> get_u64(RDAddress address) {
    spdlog::trace("get_u64({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getu64(api::to_c(state::context->memory.get()),
                                       *idx);

    return tl::nullopt;
}

tl::optional<i8> get_i8(RDAddress address) {
    spdlog::trace("get_i8({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_geti8(api::to_c(state::context->memory.get()),
                                      *idx);

    return tl::nullopt;
}

tl::optional<i16> get_i16(RDAddress address) {
    spdlog::trace("get_i16({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_geti16(api::to_c(state::context->memory.get()),
                                       *idx);

    return tl::nullopt;
}

tl::optional<i32> get_i32(RDAddress address) {
    spdlog::trace("get_i32({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_geti32(api::to_c(state::context->memory.get()),
                                       *idx);

    return tl::nullopt;
}

tl::optional<i64> get_i64(RDAddress address) {
    spdlog::trace("get_i64({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_geti64(api::to_c(state::context->memory.get()),
                                       *idx);

    return tl::nullopt;
}

tl::optional<u16> get_u16be(RDAddress address) {
    spdlog::trace("get_u16be({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getu16be(
            api::to_c(state::context->memory.get()), *idx);

    return tl::nullopt;
}

tl::optional<u32> get_u32be(RDAddress address) {
    spdlog::trace("get_u32be({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getu32be(
            api::to_c(state::context->memory.get()), *idx);

    return tl::nullopt;
}

tl::optional<u64> get_u64be(RDAddress address) {
    spdlog::trace("get_u64be({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getu64be(
            api::to_c(state::context->memory.get()), *idx);

    return tl::nullopt;
}

tl::optional<i16> get_i16be(RDAddress address) {
    spdlog::trace("get_i16be({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_geti16be(
            api::to_c(state::context->memory.get()), *idx);

    return tl::nullopt;
}

tl::optional<i32> get_i32be(RDAddress address) {
    spdlog::trace("get_i32be({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_geti32be(
            api::to_c(state::context->memory.get()), *idx);

    return tl::nullopt;
}

tl::optional<i64> get_i64be(RDAddress address) {
    spdlog::trace("get_i64be({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_geti64be(
            api::to_c(state::context->memory.get()), *idx);

    return tl::nullopt;
}

tl::optional<std::string> get_stringz(RDAddress address) {
    spdlog::trace("get_stringz({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getstringz(
            api::to_c(state::context->memory.get()), *idx);

    return tl::nullopt;
}

tl::optional<std::string> get_string(RDAddress address, usize n) {
    spdlog::trace("get_string({:x}, {})", address, n);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getstring(
            api::to_c(state::context->memory.get()), *idx, n);

    return tl::nullopt;
}

tl::optional<typing::Value> get_type(RDAddress address,
                                     std::string_view tname) {
    spdlog::trace("get_type({:x}, '{}')", address, tname);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_gettype(api::to_c(state::context->memory.get()),
                                        *idx, tname);

    return tl::nullopt;
}

tl::optional<usize> address_to_index(RDAddress address) {
    spdlog::trace("address_to_index({:x})", address);
    if(!state::context)
        return tl::nullopt;
    return state::context->address_to_index(address);
}

tl::optional<RDAddress> index_to_address(usize idx) {
    spdlog::trace("index_to_address({})", idx);
    if(!state::context)
        return tl::nullopt;
    return state::context->index_to_address(idx);
}

} // namespace redasm::api::internal
