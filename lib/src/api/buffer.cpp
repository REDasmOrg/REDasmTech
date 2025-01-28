#include "../context.h"
#include "../memory/abstractbuffer.h"
#include "../state.h"
#include "marshal.h"
#include <redasm/buffer.h>
#include <spdlog/spdlog.h>

namespace {

template<typename T, typename F>
bool rdbuffer_getvalue(const RDBuffer* self, usize idx, T* v, F getter) {
    auto res = getter(self, idx);
    res.map([&](T x) { *v = x; });
    return res.has_value();
}

} // namespace

RDBuffer* rdbuffer_getfile() {
    spdlog::trace("rdbuffer_getfile()");
    if(redasm::state::context)
        return redasm::api::to_c(redasm::state::context->program.file.get());
    return nullptr;
}

RDBuffer* rdbuffer_getmemory() {
    spdlog::trace("rdbuffer_getmemory()");
    if(redasm::state::context)
        return redasm::api::to_c(redasm::state::context->program.memory.get());
    return nullptr;
}

usize rdbuffer_getdata(const RDBuffer* self, const u8** data) {
    spdlog::trace("rdbuffer_getdata({}, {})", fmt::ptr(self), fmt::ptr(data));
    const redasm::AbstractBuffer* b = redasm::api::from_c(self);
    if(data) *data = b->raw_data();
    return b->size();
}

usize rdbuffer_getlength(const RDBuffer* self) {
    spdlog::trace("rdbuffer_getlength({})", fmt::ptr(self));
    return redasm::api::from_c(self)->size();
}

usize rdbuffer_read(const RDBuffer* self, usize idx, void* dst, usize n) {
    spdlog::trace("rdbuffer_read({}, {:x}, {}, {})", fmt::ptr(self), idx,
                  fmt::ptr(dst), n);
    const redasm::AbstractBuffer* b = redasm::api::from_c(self);
    return b->read(idx, dst, n);
}

bool rdbuffer_readstruct(const RDBuffer* self, usize idx,
                         const RDStructField* fields, RDValue* v) {
    spdlog::trace("rdbuffer_readstruct({}, {:x}, {})", fmt::ptr(self), idx,
                  fmt::ptr(fields));
    auto res = redasm::api::from_c(self)->read_struct(idx, fields);
    res.map([&](RDValue& x) {
        if(v)
            *v = x;
        else
            rdvalue_destroy(&x);
    });
    return res.has_value();
}

bool rdbuffer_getbool(const RDBuffer* self, usize idx, bool* v) {
    spdlog::trace("rdbuffer_getbool({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::api::from_c(self)->get_bool(idx);
    res.map([&](auto x) {
        if(v) *v = x;
    });
    return res.has_value();
}

bool rdbuffer_getchar(const RDBuffer* self, usize idx, char* v) {
    spdlog::trace("rdbuffer_getchar({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::api::from_c(self)->get_char(idx);
    res.map([&](auto x) {
        if(v) *v = x;
    });
    return res.has_value();
}

bool rdbuffer_getu8(const RDBuffer* self, usize idx, u8* v) {
    spdlog::trace("rdbuffer_getu8({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::api::from_c(self)->get_u8(idx);
    res.map([&](auto x) {
        if(v) *v = x;
    });
    return res.has_value();
}

bool rdbuffer_getu16(const RDBuffer* self, usize idx, u16* v) {
    spdlog::trace("rdbuffer_getu16({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::api::from_c(self)->get_u16(idx, false);
    res.map([&](auto x) {
        if(v) *v = x;
    });
    return res.has_value();
}

bool rdbuffer_getu32(const RDBuffer* self, usize idx, u32* v) {
    spdlog::trace("rdbuffer_getu32({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::api::from_c(self)->get_u32(idx, false);
    res.map([&](auto x) {
        if(v) *v = x;
    });
    return res.has_value();
}

bool rdbuffer_getu64(const RDBuffer* self, usize idx, u64* v) {
    spdlog::trace("rdbuffer_getu64({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::api::from_c(self)->get_u64(idx, false);
    res.map([&](auto x) {
        if(v) *v = x;
    });
    return res.has_value();
}

bool rdbuffer_geti8(const RDBuffer* self, usize idx, i8* v) {
    spdlog::trace("rdbuffer_geti8({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::api::from_c(self)->get_i8(idx);
    res.map([&](auto x) {
        if(v) *v = x;
    });
    return res.has_value();
}

bool rdbuffer_geti16(const RDBuffer* self, usize idx, i16* v) {
    spdlog::trace("rdbuffer_geti16({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::api::from_c(self)->get_i16(idx, false);
    res.map([&](auto x) {
        if(v) *v = x;
    });
    return res.has_value();
}

bool rdbuffer_geti32(const RDBuffer* self, usize idx, i32* v) {
    spdlog::trace("rdbuffer_geti32({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::api::from_c(self)->get_i32(idx, false);
    res.map([&](auto x) {
        if(v) *v = x;
    });
    return res.has_value();
}

bool rdbuffer_geti64(const RDBuffer* self, usize idx, i64* v) {
    spdlog::trace("rdbuffer_geti64({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::api::from_c(self)->get_i64(idx, false);
    res.map([&](auto x) {
        if(v) *v = x;
    });
    return res.has_value();
}

bool rdbuffer_getu16be(const RDBuffer* self, usize idx, u16* v) {
    spdlog::trace("rdbuffer_getu16be({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::api::from_c(self)->get_u16(idx, true);
    res.map([&](auto x) {
        if(v) *v = x;
    });
    return res.has_value();
}

bool rdbuffer_getu32be(const RDBuffer* self, usize idx, u32* v) {
    spdlog::trace("rdbuffer_getu32be({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::api::from_c(self)->get_u32(idx, true);
    res.map([&](auto x) {
        if(v) *v = x;
    });
    return res.has_value();
}

bool rdbuffer_getu64be(const RDBuffer* self, usize idx, u64* v) {
    spdlog::trace("rdbuffer_getu64be({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::api::from_c(self)->get_u64(idx, true);
    res.map([&](auto x) {
        if(v) *v = x;
    });
    return res.has_value();
}

bool rdbuffer_geti16be(const RDBuffer* self, usize idx, i16* v) {
    spdlog::trace("rdbuffer_geti16be({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::api::from_c(self)->get_i16(idx, true);
    res.map([&](auto x) {
        if(v) *v = x;
    });
    return res.has_value();
}

bool rdbuffer_geti32be(const RDBuffer* self, usize idx, i32* v) {
    spdlog::trace("rdbuffer_geti32be({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::api::from_c(self)->get_i32(idx, true);
    res.map([&](auto x) {
        if(v) *v = x;
    });
    return res.has_value();
}

bool rdbuffer_geti64be(const RDBuffer* self, usize idx, i64* v) {
    spdlog::trace("rdbuffer_geti64be({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::api::from_c(self)->get_i64(idx, true);
    res.map([&](auto x) {
        if(v) *v = x;
    });
    return res.has_value();
}

bool rdbuffer_getstrz(const RDBuffer* self, usize idx, const char** v) {
    spdlog::trace("rdbuffer_getstrz({}, {:x})", fmt::ptr(self), idx,
                  fmt::ptr(v));
    static std::string s;
    auto res = redasm::api::from_c(self)->get_str(idx);

    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdbuffer_getstr(const RDBuffer* self, usize idx, usize n, const char** v) {
    spdlog::trace("rdbuffer_getstr({}, {:x}, {})", fmt::ptr(self), idx, n,
                  fmt::ptr(v));
    static std::string s;
    auto res = redasm::api::from_c(self)->get_str(idx, n);

    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdbuffer_getwstrz(const RDBuffer* self, usize idx, const char** v) {
    spdlog::trace("rdbuffer_getstr({},{:x}, {})", fmt::ptr(self), idx,
                  fmt::ptr(v));
    static std::string s;
    auto res = redasm::api::from_c(self)->get_wstr(idx);

    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdbuffer_getwstr(const RDBuffer* self, usize idx, usize n,
                      const char** v) {
    spdlog::trace("rdbuffer_getwstr({}, {:x}, {}, {})", fmt::ptr(self), idx, n,
                  fmt::ptr(v));
    static std::string s;
    auto res = redasm::api::from_c(self)->get_wstr(idx, n);

    res.map([&](const std::string& x) {
        s = x;
        if(v) *v = s.c_str();
    });

    return res.has_value();
}

bool rdbuffer_gettypename(const RDBuffer* self, usize idx, const char* tname,
                          RDValue* v) {
    spdlog::trace("rdbuffer_gettypename({}, {:x}, '{}', {})", fmt::ptr(self),
                  idx, tname, fmt::ptr(v));
    if(!tname) return false;

    auto res = redasm::api::from_c(self)->get_type(idx, tname);
    res.map([&](const RDValue& x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdbuffer_gettype(const RDBuffer* self, usize idx, const RDType* t,
                      RDValue* v) {
    spdlog::trace("rdbuffer_gettype({}, {:x}, {}, {})", fmt::ptr(self), idx,
                  fmt::ptr(t), fmt::ptr(v));
    if(!t) return false;

    auto res = redasm::api::from_c(self)->get_type(idx, *t);
    res.map([&](const RDValue& x) {
        if(v) *v = x;
    });

    return res.has_value();
}

bool rdbuffer_collecttypename(const RDBuffer* self, usize idx,
                              const char* tname, RDValue* v) {
    spdlog::trace("rdbuffer_collecttypename({}, {:x}, '{}', {})",
                  fmt::ptr(self), idx, tname, fmt::ptr(v));
    if(!tname) return false;

    auto res = redasm::api::from_c(self)->get_type(idx, tname);

    res.map([&](RDValue& x) {
        redasm::state::context->collectedtypes.emplace_back(idx, x.type);
        if(v)
            *v = x;
        else
            rdvalue_destroy(&x);
    });

    return res.has_value();
}

bool rdbuffer_collecttype(const RDBuffer* self, usize idx, const RDType* t,
                          RDValue* v) {

    spdlog::trace("rdbuffer_collecttype({}, {:x}, {}, {})", fmt::ptr(self), idx,
                  fmt::ptr(t), fmt::ptr(v));
    if(!t) return false;

    auto res = redasm::api::from_c(self)->get_type(idx, *t);

    res.map([&](RDValue& x) {
        redasm::state::context->collectedtypes.emplace_back(idx, x.type);
        if(v)
            *v = x;
        else
            rdvalue_destroy(&x);
    });

    return res.has_value();
}
