#include "../memory/stream.h"
#include "../context.h"
#include "../memory/memorystream.h"
#include "../state.h"
#include "../typing/base.h"
#include "marshal.h"
#include <redasm/stream.h>
#include <spdlog/spdlog.h>

RDStream* rdstream_createfromfile() {
    spdlog::trace("rdstream_createfromfile()");
    return redasm::api::to_c(new redasm::Stream());
}

RDStream* rdstream_createfrommemory() {
    spdlog::trace("rdstream_createfrommemory()");
    return redasm::api::to_c(new redasm::MemoryStream());
}

usize rdstream_seek(RDStream* self, usize off) {
    spdlog::trace("rdstream_seek({}, {})", fmt::ptr(self), off);
    return redasm::api::from_c(self)->seek(off);
}

usize rdstream_move(RDStream* self, isize off) {
    spdlog::trace("rdstream_move({}, {})", fmt::ptr(self), off);
    return redasm::api::from_c(self)->move(off);
}

usize rdstream_getpos(const RDStream* self) {
    spdlog::trace("rdstream_getpos({})", fmt::ptr(self));
    return redasm::api::from_c(self)->position;
}

void rdstream_rewind(RDStream* self) {
    spdlog::trace("rdstream_rewind({})", fmt::ptr(self));
    redasm::api::from_c(self)->rewind();
}

bool rdstream_peek_type(RDStream* self, const char* tname, RDValue* v) {
    spdlog::trace("rdstream_peek_type({}, '{}', {})", fmt::ptr(self), tname,
                  fmt::ptr(v));
    if(!tname) return false;

    auto res = redasm::api::from_c(self)->peek_type(tname);

    res.map([&](RDValue& x) {
        if(v)
            *v = x;
        else
            rdvalue_destroy(&x);
    });

    return res.has_value();
}

bool rdstream_peek_strz(RDStream* self, const char** v) {
    spdlog::trace("rdstream_peek_strz({}, {})", fmt::ptr(self), fmt::ptr(v));
    static std::string s;

    auto res = redasm::api::from_c(self)->peek_str();
    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_peek_str(RDStream* self, usize n, const char** v) {
    spdlog::trace("rdstream_peek_str({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(v));
    static std::string s;

    auto res = redasm::api::from_c(self)->peek_str(n);
    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_peek_wstrz(RDStream* self, const char** v) {
    spdlog::trace("rdstream_peek_wstrz({}, {})", fmt::ptr(self), fmt::ptr(v));
    static std::string s;

    auto res = redasm::api::from_c(self)->peek_wstr();
    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_peek_wstr(RDStream* self, usize n, const char** v) {
    spdlog::trace("rdstream_peek_wstr({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(v));
    static std::string s;

    auto res = redasm::api::from_c(self)->peek_wstr(n);
    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_peek_u8(RDStream* self, u8* v) {
    spdlog::trace("rdstream_peek_u8({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->peek_u8();
    res.map([&](u8 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_u16(RDStream* self, u16* v) {
    spdlog::trace("rdstream_peek_u16({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->peek_u16();
    res.map([&](u16 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_u32(RDStream* self, u32* v) {
    spdlog::trace("rdstream_peek_u32({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->peek_u32();
    res.map([&](u32 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_u64(RDStream* self, u64* v) {
    spdlog::trace("rdstream_peek_u64({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->peek_u64();
    res.map([&](u64 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_i8(RDStream* self, i8* v) {
    spdlog::trace("rdstream_peek_i8({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->peek_i8();
    res.map([&](i8 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_i16(RDStream* self, i16* v) {
    spdlog::trace("rdstream_peek_i16({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->peek_i16();
    res.map([&](i16 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_i32(RDStream* self, i32* v) {
    spdlog::trace("rdstream_peek_i32({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->peek_i32();
    res.map([&](i32 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_i64(RDStream* self, i64* v) {
    spdlog::trace("rdstream_peek_i64({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->peek_i64();
    res.map([&](i64 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_u16be(RDStream* self, u16* v) {
    spdlog::trace("rdstream_peek_u16be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->peek_u16(true);
    res.map([&](u16 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_u32be(RDStream* self, u32* v) {
    spdlog::trace("rdstream_peek_u32be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->peek_u32(true);
    res.map([&](u32 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_u64be(RDStream* self, u64* v) {
    spdlog::trace("rdstream_peek_u64be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->peek_u64(true);
    res.map([&](u64 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_i16be(RDStream* self, i16* v) {
    spdlog::trace("rdstream_peek_i16be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->peek_i16(true);
    res.map([&](i16 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_i32be(RDStream* self, i32* v) {
    spdlog::trace("rdstream_peek_i32be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->peek_i32(true);
    res.map([&](i32 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_i64be(RDStream* self, i64* v) {
    spdlog::trace("rdstream_peek_i64be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->peek_i64(true);
    res.map([&](i64 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_type(RDStream* self, const char* tname, RDValue* v) {
    spdlog::trace("rdstream_read_type({}, '{}', {})", fmt::ptr(self),
                  fmt::ptr(tname), fmt::ptr(v));
    if(!tname || !v) return false;

    auto res = redasm::api::from_c(self)->read_type(tname);
    res.map([&](RDValue& x) {
        if(v)
            *v = x;
        else
            rdvalue_destroy(&x);
    });

    return res.has_value();
}

bool rdstream_read_strz(RDStream* self, const char** v) {
    spdlog::trace("rdstream_read_strz({}, {})", fmt::ptr(self), fmt::ptr(v));
    static std::string s;
    auto res = redasm::api::from_c(self)->read_str();

    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_read_str(RDStream* self, usize n, const char** v) {
    spdlog::trace("rdstream_read_str({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(v));
    static std::string s;
    auto res = redasm::api::from_c(self)->read_str(n);

    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_read_wstrz(RDStream* self, const char** v) {
    spdlog::trace("rdstream_read_wstrz({}, {})", fmt::ptr(self), fmt::ptr(v));
    static std::string s;
    auto res = redasm::api::from_c(self)->read_wstr();

    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_read_wstr(RDStream* self, usize n, const char** v) {
    spdlog::trace("rdstream_read_wstrz({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(v));
    static std::string s;
    auto res = redasm::api::from_c(self)->read_wstr(n);

    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_read_u8(RDStream* self, u8* v) {
    spdlog::trace("rdstream_read_u8({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->read_u8();
    res.map([&](u8 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_u16(RDStream* self, u16* v) {
    spdlog::trace("rdstream_read_u16({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->read_u16();
    res.map([&](u16 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_u32(RDStream* self, u32* v) {
    spdlog::trace("rdstream_read_u32({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->read_u32();
    res.map([&](u32 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_u64(RDStream* self, u64* v) {
    spdlog::trace("rdstream_read_u64({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->read_u64();
    res.map([&](u64 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_i8(RDStream* self, i8* v) {
    spdlog::trace("rdstream_read_i8({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->read_i8();
    res.map([&](i8 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_i16(RDStream* self, i16* v) {
    spdlog::trace("rdstream_read_i16({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->read_i16();
    res.map([&](i16 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_i32(RDStream* self, i32* v) {
    spdlog::trace("rdstream_read_i32({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->read_i32();
    res.map([&](i32 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_i64(RDStream* self, i64* v) {
    spdlog::trace("rdstream_read_i64({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->read_i64();
    res.map([&](i64 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_u16be(RDStream* self, u16* v) {
    spdlog::trace("rdstream_read_u16be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->read_u16(true);
    res.map([&](u16 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_u32be(RDStream* self, u32* v) {
    spdlog::trace("rdstream_read_u32be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->read_u32(true);
    res.map([&](u32 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_u64be(RDStream* self, u64* v) {
    spdlog::trace("rdstream_read_u64be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->read_u64(true);
    res.map([&](u64 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_i16be(RDStream* self, i16* v) {
    spdlog::trace("rdstream_read_i16be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->read_i16(true);
    res.map([&](i16 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_i32be(RDStream* self, i32* v) {
    spdlog::trace("rdstream_read_i32be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::api::from_c(self)->read_i32(true);
    res.map([&](i32 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_i64be(RDStream* self, i64* v) {
    spdlog::trace("rdstream_read_i64be({}, {})", fmt::ptr(self), fmt::ptr(v));

    auto res = redasm::api::from_c(self)->read_i64(true);
    res.map([&](i64 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_collect_type(RDStream* self, const char* tname, RDValue* v) {
    spdlog::trace("rdstream_collect_type({}, '{}', {})", fmt::ptr(self), tname,
                  fmt::ptr(v));
    if(!tname | !v) return false;

    redasm::Context* ctx = redasm::state::context;
    if(!ctx) return false;

    redasm::AbstractStream* obj = redasm::api::from_c(self);
    usize pos = obj->position;
    auto res = obj->read_type(tname);

    res.map([&](RDValue& x) {
        if(v) {
            ctx->collectedtypes.emplace_back(pos, x.type);
            *v = x;
        }
        else
            rdvalue_destroy(&x);
    });

    return res.has_value();
}

bool rdstream_collect_strz(RDStream* self, const char** v) {
    spdlog::trace("rdstream_collect_strz({}, {})", fmt::ptr(self), fmt::ptr(v));

    redasm::Context* ctx = redasm::state::context;
    if(!ctx) return false;

    static std::string s;

    redasm::AbstractStream* obj = redasm::api::from_c(self);
    usize pos = obj->position;
    auto res = obj->read_str();

    res.map([&](const std::string& x) {
        ctx->collectedtypes.emplace_back(
            pos, ctx->types.from_string(redasm::typing::names::STR));
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_collect_str(RDStream* self, usize n, const char** v) {
    spdlog::trace("rdstream_collect_str({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(v));

    redasm::Context* ctx = redasm::state::context;
    if(!ctx) return false;

    static std::string s;

    redasm::AbstractStream* obj = redasm::api::from_c(self);
    usize pos = obj->position;
    auto res = obj->read_str(n);

    res.map([&](const std::string& x) {
        RDType t = {.id = redasm::typing::ids::CHAR, .n = x.size()};
        ctx->collectedtypes.emplace_back(pos, t);
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_collect_wstrz(RDStream* self, const char** v) {
    spdlog::trace("rdstream_collect_wstr({}, {})", fmt::ptr(self), fmt::ptr(v));

    redasm::Context* ctx = redasm::state::context;
    if(!ctx) return false;

    static std::string s;

    redasm::AbstractStream* obj = redasm::api::from_c(self);
    usize pos = obj->position;
    auto res = obj->read_wstr();

    res.map([&](const std::string& x) {
        ctx->collectedtypes.emplace_back(
            pos, ctx->types.from_string(redasm::typing::names::WSTR));

        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_collect_wstr(RDStream* self, usize n, const char** v) {
    spdlog::trace("rdstream_collect_wstr({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(v));

    redasm::Context* ctx = redasm::state::context;
    if(!ctx) return false;

    static std::string s;

    redasm::AbstractStream* obj = redasm::api::from_c(self);
    usize pos = obj->position;
    auto res = obj->read_wstr(n);

    res.map([&](const std::string& x) {
        RDType t = {.id = redasm::typing::ids::WCHAR, .n = x.size()};
        ctx->collectedtypes.emplace_back(pos, t);
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}
