#include "../internal/buffer.h"
#include "../marshal.h"
#include <redasm/buffer.h>

namespace {

template<typename T, typename F>
bool rdbuffer_getvalue(const RDBuffer* self, usize idx, T* v, F getter) {
    auto res = getter(self, idx);
    res.map([&](T x) { *v = x; });
    return res.has_value();
}

} // namespace

RDBuffer* rdbuffer_getfile(void) {
    return redasm::api::internal::buffer_getfile();
}

RDBuffer* rdbuffer_getmemory(void) {
    return redasm::api::internal::buffer_getmemory();
}

bool rdbuffer_getbool(const RDBuffer* self, usize idx, bool* v) {
    return rdbuffer_getvalue(self, idx, v,
                             redasm::api::internal::buffer_getbool);
}

bool rdbuffer_getchar(const RDBuffer* self, usize idx, char* v) {
    return rdbuffer_getvalue(self, idx, v,
                             redasm::api::internal::buffer_getchar);
}

bool rdbuffer_getu8(const RDBuffer* self, usize idx, u8* v) {
    return rdbuffer_getvalue(self, idx, v, redasm::api::internal::buffer_getu8);
}

bool rdbuffer_getu16(const RDBuffer* self, usize idx, u16* v) {
    return rdbuffer_getvalue(self, idx, v,
                             redasm::api::internal::buffer_getu16);
}

bool rdbuffer_getu32(const RDBuffer* self, usize idx, u32* v) {
    return rdbuffer_getvalue(self, idx, v,
                             redasm::api::internal::buffer_getu32);
}

bool rdbuffer_getu64(const RDBuffer* self, usize idx, u64* v) {
    return rdbuffer_getvalue(self, idx, v,
                             redasm::api::internal::buffer_getu64);
}

bool rdbuffer_geti8(const RDBuffer* self, usize idx, i8* v) {
    return rdbuffer_getvalue(self, idx, v, redasm::api::internal::buffer_geti8);
}

bool rdbuffer_geti16(const RDBuffer* self, usize idx, i16* v) {
    return rdbuffer_getvalue(self, idx, v,
                             redasm::api::internal::buffer_geti16);
}

bool rdbuffer_geti32(const RDBuffer* self, usize idx, i32* v) {
    return rdbuffer_getvalue(self, idx, v,
                             redasm::api::internal::buffer_geti32);
}

bool rdbuffer_geti64(const RDBuffer* self, usize idx, i64* v) {
    return rdbuffer_getvalue(self, idx, v,
                             redasm::api::internal::buffer_geti64);
}

bool rdbuffer_getu16be(const RDBuffer* self, usize idx, u16* v) {
    return rdbuffer_getvalue(self, idx, v,
                             redasm::api::internal::buffer_getu16be);
}

bool rdbuffer_getu32be(const RDBuffer* self, usize idx, u32* v) {
    return rdbuffer_getvalue(self, idx, v,
                             redasm::api::internal::buffer_getu32be);
}

bool rdbuffer_getu64be(const RDBuffer* self, usize idx, u64* v) {
    return rdbuffer_getvalue(self, idx, v,
                             redasm::api::internal::buffer_getu64be);
}

bool rdbuffer_geti16be(const RDBuffer* self, usize idx, i16* v) {
    return rdbuffer_getvalue(self, idx, v,
                             redasm::api::internal::buffer_geti16be);
}

bool rdbuffer_geti32be(const RDBuffer* self, usize idx, i32* v) {
    return rdbuffer_getvalue(self, idx, v,
                             redasm::api::internal::buffer_geti32be);
}

bool rdbuffer_geti64be(const RDBuffer* self, usize idx, i64* v) {
    return rdbuffer_getvalue(self, idx, v,
                             redasm::api::internal::buffer_geti64be);
}

bool rdbuffer_getstrz(const RDBuffer* self, usize idx, const char** v) {
    static std::string s;
    auto res = redasm::api::internal::buffer_getstrz(self, idx);

    res.map([&](const std::string& x) {
        s = x;
        if(v)
            *v = s.c_str();
    });

    return res.has_value();
}

bool rdbuffer_getstr(const RDBuffer* self, usize idx, usize n, const char** v) {
    static std::string s;
    auto res = redasm::api::internal::buffer_getstr(self, idx, n);

    res.map([&](const std::string& x) {
        s = x;
        if(v)
            *v = s.c_str();
    });

    return res.has_value();
}

bool rdbuffer_getwstrz(const RDBuffer* self, usize idx, const char** v) {
    static std::string s;
    auto res = redasm::api::internal::buffer_getwstrz(self, idx);

    res.map([&](const std::string& x) {
        s = x;
        if(v)
            *v = s.c_str();
    });

    return res.has_value();
}

bool rdbuffer_getwstr(const RDBuffer* self, usize idx, usize n,
                      const char** v) {
    static std::string s;
    auto res = redasm::api::internal::buffer_getwstr(self, idx, n);

    res.map([&](const std::string& x) {
        s = x;
        if(v)
            *v = s.c_str();
    });

    return res.has_value();
}

bool rdbuffer_gettype(const RDBuffer* self, usize idx, const char* tname,
                      const RDValue** v) {
    if(!tname)
        return false;

    auto res = redasm::api::internal::buffer_gettype(self, idx, tname);

    res.map([&](const redasm::typing::Value& x) {
        if(v)
            *v = redasm::api::to_c(&x);
    });

    return res.has_value();
}
