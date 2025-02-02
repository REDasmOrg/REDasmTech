#include "../memory/memory.h"
#include "../context.h"
#include "../memory/stringfinder.h"
#include "../state.h"
#include "marshal.h"
#include <redasm/memory.h>
#include <spdlog/spdlog.h>

namespace {

#define return_getvalue(T, v, address)                                         \
    const redasm::Context* ctx = redasm::state::context;                       \
    if(!ctx) return false;                                                     \
    if(const RDSegment* seg = ctx->program.find_segment(address); seg) {    \
        auto res = redasm::memory::get_##T(seg, address);                      \
        res.map([&](const T& x) {                                              \
            if(v) *(v) = x;                                                    \
        });                                                                    \
        return res.has_value();                                                \
    }                                                                          \
    return false;

#define return_getvalue_b(T, v, address, b)                                    \
    const redasm::Context* ctx = redasm::state::context;                       \
    if(!ctx) return false;                                                     \
    if(const RDSegment* seg = ctx->program.find_segment(address); seg) {    \
        auto res = redasm::memory::get_##T(seg, address, b);                   \
        res.map([&](const T& x) {                                              \
            if(v) *(v) = x;                                                    \
        });                                                                    \
        return res.has_value();                                                \
    }                                                                          \
    return false;

} // namespace

void rd_memoryinfo(RDMemoryInfo* mi) {
    spdlog::trace("rd_memoryinfo({})", fmt::ptr(mi));

    if(!mi) return;

    // *mi = {
    //     .baseaddress = redasm::state::context->baseaddress,
    //     .end_baseaddress = redasm::state::context->end_baseaddress(),
    //     .size = redasm::state::context->program.memory_old->size(),
    // };
}

usize rd_read(RDAddress address, void* data, usize n) {
    spdlog::trace("rd_read({}, {}, {})", address, fmt::ptr(data), n);
    const redasm::Context* ctx = redasm::state::context;
    if(!ctx) return 0;

    if(const RDSegment* seg = ctx->program.find_segment(address); seg)
        return redasm::memory::read(seg, address, data, n);

    return 0;
}

// clang-format off
bool rd_getbool(RDAddress address, bool* v) { return_getvalue(bool, v, address); }
bool rd_getchar(RDAddress address, char* v) { return_getvalue(char, v, address); }
bool rd_getu8(RDAddress address, u8* v) { return_getvalue(u8, v, address); }
bool rd_getu16(RDAddress address, u16* v) { return_getvalue_b(u16, v, address, false); }
bool rd_getu32(RDAddress address, u32* v) { return_getvalue_b(u32, v, address, false); }
bool rd_getu64(RDAddress address, u64* v) { return_getvalue_b(u64, v, address, false); }
bool rd_geti8(RDAddress address, i8* v) { return_getvalue(i8, v, address); }
bool rd_geti16(RDAddress address, i16* v) { return_getvalue_b(i16, v, address, false); }
bool rd_geti32(RDAddress address, i32* v) { return_getvalue_b(i32, v, address, false); }
bool rd_geti64(RDAddress address, i64* v) { return_getvalue_b(i64, v, address, false); }
bool rd_getu16be(RDAddress address, u16* v) { return_getvalue_b(u16, v, address, true); }
bool rd_getu32be(RDAddress address, u32* v) { return_getvalue_b(u32, v, address, true); }
bool rd_getu64be(RDAddress address, u64* v) { return_getvalue_b(u64, v, address, true); }
bool rd_geti16be(RDAddress address, i16* v) { return_getvalue_b(i16, v, address, true); }
bool rd_geti32be(RDAddress address, i32* v) { return_getvalue_b(i32, v, address, true); }
bool rd_geti64be(RDAddress address, i64* v) { return_getvalue_b(i64, v, address, true); }
// clang-format on

const char* rd_getstr(RDAddress address, usize n) {
    spdlog::trace("rd_getstr({:x})", address);
    const redasm::Context* ctx = redasm::state::context;
    if(!ctx) return nullptr;

    static std::string res;

    if(const RDSegment* seg = ctx->program.find_segment(address); seg) {
        auto str = redasm::memory::get_str(seg, address, n);

        if(str) {
            res = *str;
            return res.c_str();
        }
    }

    return nullptr;
}

const char* rd_getwstr(RDAddress address, usize n) {
    spdlog::trace("rd_getwstr({:x})", address);
    const redasm::Context* ctx = redasm::state::context;
    if(!ctx) return nullptr;

    static std::string res;

    if(const RDSegment* seg = ctx->program.find_segment(address); seg) {
        auto str = redasm::memory::get_wstr(seg, address, n);

        if(str) {
            res = *str;
            return res.c_str();
        }
    }

    return nullptr;
}

const char* rd_getstrz(RDAddress address) {
    spdlog::trace("rd_getstrz({:x})", address);
    const redasm::Context* ctx = redasm::state::context;
    if(!ctx) return nullptr;

    static std::string res;

    if(const RDSegment* seg = ctx->program.find_segment(address); seg) {
        auto str = redasm::memory::get_str(seg, address);

        if(str) {
            res = *str;
            return res.c_str();
        }
    }

    return nullptr;
}

const char* rd_getwstrz(RDAddress address) {
    spdlog::trace("rd_getwstrz({:x})", address);
    const redasm::Context* ctx = redasm::state::context;
    if(!ctx) return nullptr;

    static std::string res;

    if(const RDSegment* seg = ctx->program.find_segment(address); seg) {
        auto str = redasm::memory::get_wstr(seg, address);

        if(str) {
            res = *str;
            return res.c_str();
        }
    }

    return nullptr;
}

bool rd_isaddress(RDAddress address) {
    spdlog::trace("rd_isaddress({:x})", address);
    const redasm::Context* ctx = redasm::state::context;
    return ctx && ctx->program.find_segment(address) != nullptr;
}

bool rd_checkstring(RDAddress address, RDStringResult* r) {
    spdlog::trace("rd_checkstring({:x}, {})", address, fmt::ptr(r));
    auto res = redasm::stringfinder::classify(address);
    if(res && r) *r = *res;
    return res.has_value();
}
