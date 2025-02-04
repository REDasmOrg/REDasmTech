#include "../memory/buffer.h"
#include "../internal/buffer_internal.h"
#include "../utils/utils.h"
#include <fstream>
#include <redasm/buffer.h>
#include <redasm/byte.h>
#include <spdlog/spdlog.h>

RDBuffer* rdbuffer_createfile(const char* filepath) {
    spdlog::trace("rdbuffer_createfile('{}')", filepath);
    if(!filepath) return nullptr;

    std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);

    if(!ifs.is_open()) {
        spdlog::error("Cannot open '{}'", filepath);
        return {};
    }

    auto* self = new RDBuffer{
        .data = reinterpret_cast<u8*>(std::calloc(ifs.tellg(), sizeof(u8))),
        .length = static_cast<usize>(ifs.tellg()),
        .source = redasm::utils::copy_str(filepath),

        .get_byte =
            [](const RDBuffer* self, usize idx, u8* b) {
                if(idx < self->length) {
                    if(b) *b = reinterpret_cast<u8*>(self->data)[idx];
                    return true;
                }

                return false;
            },
    };

    ifs.seekg(0);
    ifs.read(reinterpret_cast<char*>(self->data), self->length);
    return self;
}

RDBuffer* rdbuffer_creatememory(usize n) {
    spdlog::trace("rdbuffer_creatememory({:x})", n);
    if(!n) return nullptr;

    auto* self = new RDBuffer{
        .m_data = reinterpret_cast<RDMByte*>(std::calloc(n, sizeof(RDMByte))),
        .length = n,
        .source = redasm::utils::copy_str("MEMORY"),

        .get_byte =
            [](const RDBuffer* self, usize idx, u8* b) {
                return idx < self->length &&
                       rdmbyte_getbyte(self->m_data[idx], b);
            },
    };

    return self;
}

usize rdbuffer_getlength(const RDBuffer* self) {
    if(self) return self->length;
    return 0;
}

const char* rdbuffer_getsource(const RDBuffer* self) {
    if(self) return self->source;
    return nullptr;
}

const u8* rdbuffer_getdata(const RDBuffer* self) {
    if(self) return self->data;
    return nullptr;
}

const RDMByte* rdbuffer_getmdata(const RDBuffer* self) {
    if(self) return self->m_data;
    return nullptr;
}

usize rdbuffer_read(const RDBuffer* self, usize idx, void* dst, usize n) {
    spdlog::trace("rdbuffer_read({}, {:x}, {}, {})", fmt::ptr(self), idx,
                  fmt::ptr(dst), n);
    return redasm::buffer::read(self, idx, dst, n);
}

bool rdbuffer_readstruct(const RDBuffer* self, usize idx,
                         const RDStructField* fields, RDValue* v) {
    spdlog::trace("rdbuffer_readstruct({}, {:x}, {})", fmt::ptr(self), idx,
                  fmt::ptr(fields));
    auto res = redasm::buffer::read_struct(self, idx, fields);
    if(res) {
        if(v)
            *v = *res;
        else
            rdvalue_destroy(&res.value());
    }
    return res.has_value();
}

bool rdbuffer_getbool(const RDBuffer* self, usize idx, bool* v) {
    spdlog::trace("rdbuffer_getbool({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::buffer::get_bool(self, idx);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_getchar(const RDBuffer* self, usize idx, char* v) {
    spdlog::trace("rdbuffer_getchar({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::buffer::get_char(self, idx);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_getu8(const RDBuffer* self, usize idx, u8* v) {
    spdlog::trace("rdbuffer_getu8({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::buffer::get_char(self, idx);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_getu16(const RDBuffer* self, usize idx, u16* v) {
    spdlog::trace("rdbuffer_getu16({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::buffer::get_u16(self, idx, false);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_getu32(const RDBuffer* self, usize idx, u32* v) {
    spdlog::trace("rdbuffer_getu32({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::buffer::get_u32(self, idx, false);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_getu64(const RDBuffer* self, usize idx, u64* v) {
    spdlog::trace("rdbuffer_getu64({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::buffer::get_u64(self, idx, false);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_geti8(const RDBuffer* self, usize idx, i8* v) {
    spdlog::trace("rdbuffer_geti8({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::buffer::get_i8(self, idx);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_geti16(const RDBuffer* self, usize idx, i16* v) {
    spdlog::trace("rdbuffer_geti16({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::buffer::get_i16(self, idx, false);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_geti32(const RDBuffer* self, usize idx, i32* v) {
    spdlog::trace("rdbuffer_geti32({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::buffer::get_i32(self, idx, false);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_geti64(const RDBuffer* self, usize idx, i64* v) {
    spdlog::trace("rdbuffer_geti64({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::buffer::get_i64(self, idx, false);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_getu16be(const RDBuffer* self, usize idx, u16* v) {
    spdlog::trace("rdbuffer_getu16be({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::buffer::get_u16(self, idx, true);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_getu32be(const RDBuffer* self, usize idx, u32* v) {
    spdlog::trace("rdbuffer_getu32be({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::buffer::get_u32(self, idx, true);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_getu64be(const RDBuffer* self, usize idx, u64* v) {
    spdlog::trace("rdbuffer_getu64be({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::buffer::get_u64(self, idx, true);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_geti16be(const RDBuffer* self, usize idx, i16* v) {
    spdlog::trace("rdbuffer_geti16be({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::buffer::get_i16(self, idx, true);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_geti32be(const RDBuffer* self, usize idx, i32* v) {
    spdlog::trace("rdbuffer_geti32be({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::buffer::get_i32(self, idx, true);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_geti64be(const RDBuffer* self, usize idx, i64* v) {
    spdlog::trace("rdbuffer_geti64be({}, {:x})", fmt::ptr(self), idx);
    auto res = redasm::buffer::get_i64(self, idx, true);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_getstrz(const RDBuffer* self, usize idx, const char** v) {
    spdlog::trace("rdbuffer_getstrz({}, {:x})", fmt::ptr(self), idx,
                  fmt::ptr(v));
    static std::string s;
    auto res = redasm::buffer::get_str(self, idx);
    if(res && v) {
        s = *res;
        *v = s.c_str();
    }
    return res.has_value();
}

bool rdbuffer_getstr(const RDBuffer* self, usize idx, usize n, const char** v) {
    spdlog::trace("rdbuffer_getstr({}, {:x}, {})", fmt::ptr(self), idx, n,
                  fmt::ptr(v));
    static std::string s;
    auto res = redasm::buffer::get_str(self, idx, n);
    if(res && v) {
        s = *res;
        *v = s.c_str();
    }
    return res.has_value();
}

bool rdbuffer_getwstrz(const RDBuffer* self, usize idx, const char** v) {
    spdlog::trace("rdbuffer_getstr({},{:x}, {})", fmt::ptr(self), idx,
                  fmt::ptr(v));
    static std::string s;
    auto res = redasm::buffer::get_wstr(self, idx);
    if(res && v) {
        s = *res;
        *v = s.c_str();
    }
    return res.has_value();
}

bool rdbuffer_getwstr(const RDBuffer* self, usize idx, usize n,
                      const char** v) {
    spdlog::trace("rdbuffer_getwstr({}, {:x}, {}, {})", fmt::ptr(self), idx, n,
                  fmt::ptr(v));
    static std::string s;
    auto res = redasm::buffer::get_wstr(self, idx, n);
    if(res && v) {
        s = *res;
        *v = s.c_str();
    }
    return res.has_value();
}

bool rdbuffer_gettypename(const RDBuffer* self, usize idx, const char* tname,
                          RDValue* v) {
    spdlog::trace("rdbuffer_gettypename({}, {:x}, '{}', {})", fmt::ptr(self),
                  idx, tname, fmt::ptr(v));
    if(!tname) return false;

    auto res = redasm::buffer::get_type(self, idx, tname);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdbuffer_gettype(const RDBuffer* self, usize idx, const RDType* t,
                      RDValue* v) {
    spdlog::trace("rdbuffer_gettype({}, {:x}, {}, {})", fmt::ptr(self), idx,
                  fmt::ptr(t), fmt::ptr(v));
    if(!t) return false;

    auto res = redasm::buffer::get_type(self, idx, *t);
    if(res) {
        if(v)
            *v = *res;
        else
            rdvalue_destroy(&res.value());
    }
    return res.has_value();
}

void rdbuffer_destroy(RDBuffer* self) {
    spdlog::trace("rdbuffer_destroy({})", fmt::ptr(self));
    if(!self) return;
    std::free(self->data);
    delete[] self->source;
    self->source = nullptr;
    self->data = nullptr;
    self->length = 0;
    delete self;
}
