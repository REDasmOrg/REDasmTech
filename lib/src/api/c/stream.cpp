#include "../internal/stream.h"
#include <redasm/stream.h>

RDStream* rdstream_createfromfile() {
    return redasm::api::internal::stream_createfromfile();
}

RDStream* rdstream_createfrommemory() {
    return redasm::api::internal::stream_createfrommemory();
}

usize rdstream_seek(RDStream* self, usize off) {
    return redasm::api::internal::stream_seek(self, off);
}

usize rdstream_move(RDStream* self, isize off) {
    return redasm::api::internal::stream_move(self, off);
}

usize rdstream_getpos(const RDStream* self) {
    return redasm::api::internal::stream_getpos(self);
}

void rdstream_rewind(RDStream* self) {
    redasm::api::internal::stream_rewind(self);
}

bool rdstream_peek_type(RDStream* self, const char* tname, RDValue* v) {
    if(!tname) return false;

    auto res = redasm::api::internal::stream_peek_type(self, tname);
    res.map([&](RDValue& x) {
        if(v)
            *v = x;
        else
            rdvalue_destroy(&x);
    });

    return res.has_value();
}

bool rdstream_peek_strz(RDStream* self, const char** v) {
    static std::string s;

    auto res = redasm::api::internal::stream_peek_strz(self);
    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_peek_str(RDStream* self, usize n, const char** v) {
    static std::string s;

    auto res = redasm::api::internal::stream_peek_str(self, n);
    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_peek_wstrz(RDStream* self, const char** v) {
    static std::string s;

    auto res = redasm::api::internal::stream_peek_wstrz(self);
    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_peek_wstr(RDStream* self, usize n, const char** v) {
    static std::string s;

    auto res = redasm::api::internal::stream_peek_wstr(self, n);
    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_peek_u8(RDStream* self, u8* v) {
    auto res = redasm::api::internal::stream_peek_u8(self);
    res.map([&](u8 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_u16(RDStream* self, u16* v) {
    auto res = redasm::api::internal::stream_peek_u16(self);
    res.map([&](u16 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_u32(RDStream* self, u32* v) {
    auto res = redasm::api::internal::stream_peek_u32(self);
    res.map([&](u32 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_u64(RDStream* self, u64* v) {
    auto res = redasm::api::internal::stream_peek_u64(self);
    res.map([&](u64 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_i8(RDStream* self, i8* v) {
    auto res = redasm::api::internal::stream_peek_i8(self);
    res.map([&](i8 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_i16(RDStream* self, i16* v) {
    auto res = redasm::api::internal::stream_peek_i16(self);
    res.map([&](i16 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_i32(RDStream* self, i32* v) {
    auto res = redasm::api::internal::stream_peek_i32(self);
    res.map([&](i32 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_i64(RDStream* self, i64* v) {
    auto res = redasm::api::internal::stream_peek_i32(self);
    res.map([&](i64 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_u16be(RDStream* self, u16* v) {
    auto res = redasm::api::internal::stream_peek_u16be(self);
    res.map([&](u16 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_u32be(RDStream* self, u32* v) {
    auto res = redasm::api::internal::stream_peek_u32be(self);
    res.map([&](u32 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_u64be(RDStream* self, u64* v) {
    auto res = redasm::api::internal::stream_peek_u64be(self);
    res.map([&](u64 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_i16be(RDStream* self, i16* v) {
    auto res = redasm::api::internal::stream_peek_i16be(self);
    res.map([&](i16 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_i32be(RDStream* self, i32* v) {
    auto res = redasm::api::internal::stream_peek_i32be(self);
    res.map([&](i32 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_peek_i64be(RDStream* self, i64* v) {
    auto res = redasm::api::internal::stream_peek_i64be(self);
    res.map([&](i64 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_type(RDStream* self, const char* tname, RDValue* v) {
    if(!tname || !v) return false;

    auto res = redasm::api::internal::stream_read_type(self, tname);
    res.map([&](RDValue& x) {
        if(v)
            *v = x;
        else
            rdvalue_destroy(&x);
    });

    return res.has_value();
}

bool rdstream_read_strz(RDStream* self, const char** v) {
    static std::string s;

    auto res = redasm::api::internal::stream_read_strz(self);
    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_read_str(RDStream* self, usize n, const char** v) {
    static std::string s;

    auto res = redasm::api::internal::stream_read_str(self, n);
    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_read_wstrz(RDStream* self, const char** v) {
    static std::string s;

    auto res = redasm::api::internal::stream_read_wstrz(self);
    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_read_wstr(RDStream* self, usize n, const char** v) {
    static std::string s;

    auto res = redasm::api::internal::stream_read_wstr(self, n);
    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_read_u8(RDStream* self, u8* v) {
    auto res = redasm::api::internal::stream_read_u8(self);
    res.map([&](u8 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_u16(RDStream* self, u16* v) {
    auto res = redasm::api::internal::stream_read_u16(self);
    res.map([&](u16 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_u32(RDStream* self, u32* v) {
    auto res = redasm::api::internal::stream_read_u32(self);
    res.map([&](u32 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_u64(RDStream* self, u64* v) {
    auto res = redasm::api::internal::stream_read_u64(self);
    res.map([&](u64 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_i8(RDStream* self, i8* v) {
    auto res = redasm::api::internal::stream_read_i8(self);
    res.map([&](i8 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_i16(RDStream* self, i16* v) {
    auto res = redasm::api::internal::stream_read_i16(self);
    res.map([&](i16 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_i32(RDStream* self, i32* v) {
    auto res = redasm::api::internal::stream_read_i32(self);
    res.map([&](i32 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_i64(RDStream* self, i64* v) {
    auto res = redasm::api::internal::stream_read_i32(self);
    res.map([&](i64 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_u16be(RDStream* self, u16* v) {
    auto res = redasm::api::internal::stream_read_u16be(self);
    res.map([&](u16 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_u32be(RDStream* self, u32* v) {
    auto res = redasm::api::internal::stream_read_u32be(self);
    res.map([&](u32 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_u64be(RDStream* self, u64* v) {
    auto res = redasm::api::internal::stream_read_u64be(self);
    res.map([&](u64 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_i16be(RDStream* self, i16* v) {
    auto res = redasm::api::internal::stream_read_i16be(self);
    res.map([&](i16 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_i32be(RDStream* self, i32* v) {
    auto res = redasm::api::internal::stream_read_i32be(self);
    res.map([&](i32 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_read_i64be(RDStream* self, i64* v) {
    auto res = redasm::api::internal::stream_read_i64be(self);
    res.map([&](i64 x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdstream_collect_type(RDStream* self, const char* tname, RDValue* v) {
    if(!tname | !v) return false;

    auto res = redasm::api::internal::stream_collect_type(self, tname);
    res.map([&](RDValue& x) {
        if(v)
            *v = x;
        else
            rdvalue_destroy(&x);
    });

    return res.has_value();
}

bool rdstream_collect_strz(RDStream* self, const char** v) {
    static std::string s;

    auto res = redasm::api::internal::stream_collect_strz(self);
    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_collect_str(RDStream* self, usize n, const char** v) {
    static std::string s;

    auto res = redasm::api::internal::stream_collect_str(self, n);
    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_collect_wstrz(RDStream* self, const char** v) {
    static std::string s;

    auto res = redasm::api::internal::stream_collect_wstrz(self);
    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdstream_collect_wstr(RDStream* self, usize n, const char** v) {
    static std::string s;

    auto res = redasm::api::internal::stream_collect_wstr(self, n);
    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}
