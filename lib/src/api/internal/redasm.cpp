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
#include <cctype>
#include <fstream>
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

bool init() {
    spdlog::trace("init()");

    if(initialized)
        return true;

    initialized = true;

    if(!state::onerror)
        state::onerror = [](std::string_view arg) { spdlog::error("{}", arg); };

    builtins::register_loaders();
    builtins::register_processors();
    builtins::register_analyzers();
    redasm::load_modules();

    if(python::init()) {
        python::main();
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

    switch(l) {
        case LOGLEVEL_TRACE: spdlog::set_level(spdlog::level::trace); break;
        case LOGLEVEL_INFO: spdlog::set_level(spdlog::level::info); break;
        case LOGLEVEL_WARNING: spdlog::set_level(spdlog::level::warn); break;
        case LOGLEVEL_ERROR: spdlog::set_level(spdlog::level::err); break;

        case LOGLEVEL_CRITICAL:
            spdlog::set_level(spdlog::level::critical);
            break;

        case LOGLEVEL_OFF: spdlog::set_level(spdlog::level::off); break;
        default: spdlog::error("set_loglevel(): Invalid log-level"); break;
    }
}

std::string symbolize(std::string s) {
    for(char& ch : s) {
        if(std::isalnum(ch) || ch == '_')
            continue;

        ch = '_';
    }

    return s;
}

void add_search_path(const std::string& p) {
    spdlog::trace("add_search_path('{}')", p);
    redasm::add_search_path(p);
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
        state::context = ctx; // Set context as active

        if(ldr.init && ldr.init(&ldr)) {
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

void enqueue(RDAddress address) {
    spdlog::trace("enqueue({:x})", address);

    if(!state::context)
        return;

    if(auto idx = state::context->address_to_index(address); idx)
        state::context->disassembler.emulator.enqueue(*idx);
}

void schedule(RDAddress address) {
    spdlog::trace("schedule({:x})", address);

    if(!state::context)
        return;

    if(auto idx = state::context->address_to_index(address); idx)
        state::context->disassembler.emulator.schedule(*idx);
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

std::string to_hex_n(usize v, usize n) {
    if(state::context)
        return state::context->to_hex(v, n);
    return {};
}

std::string to_hex(usize v) {
    if(state::context)
        return state::context->to_hex(v);
    return {};
}

usize get_segments(const RDSegment** segments) {
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

std::vector<RDRef> get_references(RDAddress address) {
    if(auto idx = state::context->address_to_index(address); idx) {
        if(!state::context->memory->at(*idx).has(BF_REFS))
            return {};

        const AddressDetail& d = state::context->database.get_detail(*idx);
        return api::to_c(d.refs);
    }

    return {};
}

usize get_bytes(const RDByte** bytes) {
    spdlog::trace("get_bytes({})", fmt::ptr(bytes));

    const Memory* m = state::context->memory.get();

    if(!m)
        return 0;

    if(*bytes)
        *bytes = reinterpret_cast<const RDByte*>(m->data());

    return m->size();
}

usize get_bits() {
    spdlog::trace("get_bits()");

    if(!state::context)
        return 0;

    return state::context->bits;
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

bool set_name(RDAddress address, const std::string& name) {
    spdlog::trace("set_name({:x}, '{}')", address, name);

    if(!state::context)
        return false;

    if(auto idx = state::context->address_to_index(address); idx) {
        state::context->set_name(*idx, name);
        return true;
    }

    return false;
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
        if(it->type == ListingItemType::INSTRUCTION)
            break;

        it++;
    }

    if(it == state::context->listing.end() || it->index != *idx)
        return {};

    usize lidx = std::distance(state::context->listing.cbegin(), it);

    Surface s{SURFACE_TEXT};
    s.render(lidx, 1);

    return std::string{s.get_text()};
}

tl::optional<typing::Value> set_type(RDAddress address,
                                     std::string_view tname) {
    spdlog::trace("set_type({:x}, '{}')", address, tname);

    auto idx = state::context->address_to_index(address);
    if(!idx)
        return tl::nullopt;

    if(!state::context->set_type(*idx, tname))
        return tl::nullopt;

    return state::context->memory->get_type(*idx, tname);
}

tl::optional<typing::Value> set_type_as(RDAddress address,
                                        std::string_view tname,
                                        const std::string& dbname) {
    spdlog::trace("set_type_as({:x}, '{}', '{}')", address, tname, dbname);

    auto idx = state::context->address_to_index(address);
    if(!idx)
        return tl::nullopt;

    if(!state::context->set_type(*idx, tname, dbname))
        return tl::nullopt;

    return state::context->memory->get_type(*idx, tname);
}

bool set_export(RDAddress address) {
    spdlog::trace("set_export({:x})", address);

    if(auto idx = state::context->address_to_index(address); idx) {
        state::context->set_export(*idx);
        return true;
    }

    return false;
}

bool set_import(RDAddress address) {
    spdlog::trace("set_import({:x})", address);

    if(auto idx = state::context->address_to_index(address); idx) {
        state::context->set_import(*idx);
        return true;
    }

    return false;
}

bool set_function_as(RDAddress address, const std::string& name) {
    spdlog::trace("set_function_as({:x}, '{}')", address, name);

    if(auto idx = state::context->address_to_index(address); idx)
        return state::context->set_function(*idx, name);

    return false;
}

bool set_function(RDAddress address) {
    return internal::set_function_as(address, {});
}

bool set_entry(RDAddress address, const std::string& name) {
    spdlog::trace("set_entry({:x}, '{}')", address, name);

    if(auto idx = state::context->address_to_index(address); idx)
        return state::context->set_entry(*idx, name);

    return false;
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

tl::optional<RDOffset> address_to_offset(RDAddress address) {
    spdlog::trace("address_to_offset({:x})", address);
    if(!state::context)
        return tl::nullopt;

    usize idx = address - state::context->baseaddress;
    if(idx >= state::context->memory->size())
        return {};

    RDOffset offset = idx;

    for(const Segment& s : state::context->segments) {
        if(idx >= s.index && idx < s.endindex) {
            offset = (idx - s.index) + s.offset;
            break;
        }
    }

    return offset;
}

tl::optional<RDAddress> offset_to_address(RDOffset offset) {
    spdlog::trace("offset_to_address({:x})", offset);
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
