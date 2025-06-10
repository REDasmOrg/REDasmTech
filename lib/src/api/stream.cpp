#include "../internal/stream_internal.h"
#include "../memory/buffer.h"
#include <redasm/stream.h>
#include <redasm/typing.h>
#include <spdlog/spdlog.h>

RDStream* rdstream_create(RDBuffer* buffer) {
    spdlog::trace("rdstream_create({})", fmt::ptr(buffer));
    if(!buffer) return nullptr;

    return new RDStream{
        .buffer = buffer,
        .position = 0,
    };
}

void rdstream_destroy(RDStream* s) { delete s; }

bool rdstream_atend(const RDStream* self) {
    return !self || self->position >= self->buffer->length;
}

usize rdstream_getpos(const RDStream* self) { return self->position; }

usize rdstream_seek(RDStream* self, usize off) {
    spdlog::trace("rdstream_seek({}, {})", fmt::ptr(self), off);

    usize oldpos = self->position;
    self->position = std::min(off, self->buffer->length);
    return oldpos;
}

usize rdstream_move(RDStream* self, isize off) {
    spdlog::trace("rdstream_move({}, {})", fmt::ptr(self), off);
    return rdstream_seek(self, self->position + off);
}

void rdstream_rewind(RDStream* self) {
    spdlog::trace("rdstream_rewind({})", fmt::ptr(self));
    self->position = 0;
}

RDValueOpt rdstream_peek_struct_n(RDStream* self, usize n,
                                  const RDStructFieldDecl* fields) {
    spdlog::trace("rdstream_peek_struct_n({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(fields));
    auto res =
        redasm::buffer::read_struct_n(self->buffer, self->position, n, fields);

    if(res) return RDValueOpt_some(*res);
    return RDValueOpt_none();
}

RDValueOpt rdstream_peek_struct(RDStream* self,
                                const RDStructFieldDecl* fields) {
    spdlog::trace("rdstream_peek_struct({}, {})", fmt::ptr(self),
                  fmt::ptr(fields));
    auto res =
        redasm::buffer::read_struct(self->buffer, self->position, fields);
    if(res) return RDValueOpt_some(*res);
    return RDValueOpt_none();
}

RDValueOpt rdstream_peek_type(const RDStream* self, const char* tname) {
    spdlog::trace("rdstream_peek_type({}, '{}')", fmt::ptr(self), tname);
    if(!tname) return RDValueOpt_none();
    auto res = redasm::buffer::get_type(self->buffer, self->position, tname);
    if(res) return RDValueOpt_some(*res);
    return RDValueOpt_none();
}

bool rdstream_peek_strz(const RDStream* self, const char** v) {
    spdlog::trace("rdstream_peek_strz({}, {})", fmt::ptr(self), fmt::ptr(v));
    static std::string s;

    auto res = redasm::buffer::get_str(self->buffer, self->position);
    if(res) {
        s = *res;
        if(v) *v = s.c_str();
    }
    return res.has_value();
}

bool rdstream_peek_str(const RDStream* self, usize n, const char** v) {
    spdlog::trace("rdstream_peek_str({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(v));
    static std::string s;

    auto res = redasm::buffer::get_str(self->buffer, self->position, n);
    if(res) {
        s = *res;
        if(v) *v = s.c_str();
    }
    return res.has_value();
}

bool rdstream_peek_wstrz(const RDStream* self, const char** v) {
    spdlog::trace("rdstream_peek_wstrz({}, {})", fmt::ptr(self), fmt::ptr(v));
    static std::string s;

    auto res = redasm::buffer::get_wstr(self->buffer, self->position);
    if(res) {
        s = *res;
        if(v) *v = s.c_str();
    }
    return res.has_value();
}

bool rdstream_peek_wstr(const RDStream* self, usize n, const char** v) {
    spdlog::trace("rdstream_peek_wstr({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(v));
    static std::string s;

    auto res = redasm::buffer::get_wstr(self->buffer, self->position, n);
    if(res) {
        s = *res;
        if(v) *v = s.c_str();
    }
    return res.has_value();
}

bool rdstream_peek_u8(const RDStream* self, u8* v) {
    spdlog::trace("rdstream_peek_u8({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::buffer::get_u8(self->buffer, self->position);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdstream_peek_u16(const RDStream* self, u16* v) {
    spdlog::trace("rdstream_peek_u16({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::buffer::get_u16(self->buffer, self->position, false);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdstream_peek_u32(const RDStream* self, u32* v) {
    spdlog::trace("rdstream_peek_u32({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::buffer::get_u32(self->buffer, self->position, false);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdstream_peek_u64(const RDStream* self, u64* v) {
    spdlog::trace("rdstream_peek_u64({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::buffer::get_u64(self->buffer, self->position, false);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdstream_peek_i8(const RDStream* self, i8* v) {
    spdlog::trace("rdstream_peek_i8({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::buffer::get_i8(self->buffer, self->position);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdstream_peek_i16(const RDStream* self, i16* v) {
    spdlog::trace("rdstream_peek_i16({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::buffer::get_i16(self->buffer, self->position, false);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdstream_peek_i32(const RDStream* self, i32* v) {
    spdlog::trace("rdstream_peek_i32({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::buffer::get_i32(self->buffer, self->position, false);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdstream_peek_i64(const RDStream* self, i64* v) {
    spdlog::trace("rdstream_peek_i64({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::buffer::get_i64(self->buffer, self->position, false);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdstream_peek_u16be(const RDStream* self, u16* v) {
    spdlog::trace("rdstream_peek_u16be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::buffer::get_u16(self->buffer, self->position, true);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdstream_peek_u32be(const RDStream* self, u32* v) {
    spdlog::trace("rdstream_peek_u32be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::buffer::get_u32(self->buffer, self->position, true);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdstream_peek_u64be(const RDStream* self, u64* v) {
    spdlog::trace("rdstream_peek_u64be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::buffer::get_u64(self->buffer, self->position, true);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdstream_peek_i16be(const RDStream* self, i16* v) {
    spdlog::trace("rdstream_peek_i16be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::buffer::get_i16(self->buffer, self->position, true);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdstream_peek_i32be(const RDStream* self, i32* v) {
    spdlog::trace("rdstream_peek_i32be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::buffer::get_i32(self->buffer, self->position, true);
    if(res && v) *v = *res;
    return res.has_value();
}

bool rdstream_peek_i64be(const RDStream* self, i64* v) {
    spdlog::trace("rdstream_peek_i64be({}, {})", fmt::ptr(self), fmt::ptr(v));
    auto res = redasm::buffer::get_i64(self->buffer, self->position, true);
    if(res && v) *v = *res;
    return res.has_value();
}

RDValueOpt rdstream_read_struct_n(RDStream* self, usize n,
                                  const RDStructFieldDecl* fields) {
    spdlog::trace("rdstream_read_struct_n({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(fields));
    usize pos = self->position;
    auto res = redasm::buffer::read_struct_n(self->buffer, pos, n, fields, pos);
    if(res) {
        self->position = pos;
        return RDValueOpt_some(*res);
    }
    return RDValueOpt_none();
}

RDValueOpt rdstream_read_struct(RDStream* self,
                                const RDStructFieldDecl* fields) {
    spdlog::trace("rdstream_read_struct({}, {})", fmt::ptr(self),
                  fmt::ptr(fields));
    usize pos = self->position;
    auto res = redasm::buffer::read_struct(self->buffer, pos, fields, pos);
    if(res) {
        self->position = pos;
        return RDValueOpt_some(*res);
    }
    return RDValueOpt_none();
}

RDValueOpt rdstream_read_type(RDStream* self, const char* tname) {
    spdlog::trace("rdstream_read_type({}, '{}')", fmt::ptr(self), tname);
    if(!tname) return RDValueOpt_none();

    usize pos = self->position;
    auto res = redasm::buffer::get_type(self->buffer, pos, tname, pos);
    if(res) {
        self->position = pos;
        return RDValueOpt_some(*res);
    }
    return RDValueOpt_none();
}

bool rdstream_read_strz(RDStream* self, const char** v) {
    spdlog::trace("rdstream_read_strz({}, {})", fmt::ptr(self), fmt::ptr(v));
    static std::string s;
    auto res = redasm::buffer::get_str(self->buffer, self->position);
    if(res) {
        self->position += res->size();
        if(v) {
            s = *res;
            *v = s.c_str();
        }
    }
    return res.has_value();
}

bool rdstream_read_str(RDStream* self, usize n, const char** v) {
    spdlog::trace("rdstream_read_str({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(v));
    static std::string s;
    auto res = redasm::buffer::get_str(self->buffer, self->position, n);
    if(res) {
        self->position += res->size();
        if(v) {
            s = *res;
            *v = s.c_str();
        }
    }
    return res.has_value();
}

bool rdstream_read_wstrz(RDStream* self, const char** v) {
    spdlog::trace("rdstream_read_wstrz({}, {})", fmt::ptr(self), fmt::ptr(v));
    static std::string s;
    auto res = redasm::buffer::get_wstr(self->buffer, self->position);
    if(res) {
        self->position += res->size();
        if(v) {
            s = *res;
            *v = s.c_str();
        }
    }
    return res.has_value();
}

bool rdstream_read_wstr(RDStream* self, usize n, const char** v) {
    spdlog::trace("rdstream_read_wstrz({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(v));
    static std::string s;
    auto res = redasm::buffer::get_wstr(self->buffer, self->position, n);
    if(res) {
        self->position += res->size();
        if(v) {
            s = *res;
            *v = s.c_str();
        }
    }
    return res.has_value();
}

bool rdstream_read_u8(RDStream* self, u8* v) {
    spdlog::trace("rdstream_read_u8({}, {})", fmt::ptr(self), fmt::ptr(v));
    if(rdstream_peek_u8(self, v)) {
        self->position += sizeof(u8);
        return true;
    }
    return false;
}

bool rdstream_read_u16(RDStream* self, u16* v) {
    spdlog::trace("rdstream_read_u16({}, {})", fmt::ptr(self), fmt::ptr(v));
    if(rdstream_peek_u16(self, v)) {
        self->position += sizeof(u16);
        return true;
    }
    return false;
}

bool rdstream_read_u32(RDStream* self, u32* v) {
    spdlog::trace("rdstream_read_u32({}, {})", fmt::ptr(self), fmt::ptr(v));
    if(rdstream_peek_u32(self, v)) {
        self->position += sizeof(u32);
        return true;
    }
    return false;
}

bool rdstream_read_u64(RDStream* self, u64* v) {
    spdlog::trace("rdstream_read_u64({}, {})", fmt::ptr(self), fmt::ptr(v));
    if(rdstream_peek_u64(self, v)) {
        self->position += sizeof(u64);
        return true;
    }
    return false;
}

bool rdstream_read_i8(RDStream* self, i8* v) {
    spdlog::trace("rdstream_read_i8({}, {})", fmt::ptr(self), fmt::ptr(v));
    if(rdstream_peek_i8(self, v)) {
        self->position += sizeof(i8);
        return true;
    }
    return false;
}

bool rdstream_read_i16(RDStream* self, i16* v) {
    spdlog::trace("rdstream_read_i16({}, {})", fmt::ptr(self), fmt::ptr(v));
    if(rdstream_peek_i16(self, v)) {
        self->position += sizeof(i16);
        return true;
    }
    return false;
}

bool rdstream_read_i32(RDStream* self, i32* v) {
    spdlog::trace("rdstream_read_i32({}, {})", fmt::ptr(self), fmt::ptr(v));
    if(rdstream_peek_i32(self, v)) {
        self->position += sizeof(i32);
        return true;
    }
    return false;
}

bool rdstream_read_i64(RDStream* self, i64* v) {
    spdlog::trace("rdstream_read_i64({}, {})", fmt::ptr(self), fmt::ptr(v));
    if(rdstream_peek_i64(self, v)) {
        self->position += sizeof(i64);
        return true;
    }
    return false;
}

bool rdstream_read_u16be(RDStream* self, u16* v) {
    spdlog::trace("rdstream_read_u16be({}, {})", fmt::ptr(self), fmt::ptr(v));
    if(rdstream_peek_u16be(self, v)) {
        self->position += sizeof(u16);
        return true;
    }
    return false;
}

bool rdstream_read_u32be(RDStream* self, u32* v) {
    spdlog::trace("rdstream_read_u32be({}, {})", fmt::ptr(self), fmt::ptr(v));
    if(rdstream_peek_u32be(self, v)) {
        self->position += sizeof(u32);
        return true;
    }
    return false;
}

bool rdstream_read_u64be(RDStream* self, u64* v) {
    spdlog::trace("rdstream_read_u64be({}, {})", fmt::ptr(self), fmt::ptr(v));
    if(rdstream_peek_u64be(self, v)) {
        self->position += sizeof(u64);
        return true;
    }
    return false;
}

bool rdstream_read_i16be(RDStream* self, i16* v) {
    spdlog::trace("rdstream_read_i16be({}, {})", fmt::ptr(self), fmt::ptr(v));
    if(rdstream_peek_i16be(self, v)) {
        self->position += sizeof(i16);
        return true;
    }
    return false;
}

bool rdstream_read_i32be(RDStream* self, i32* v) {
    spdlog::trace("rdstream_read_i32be({}, {})", fmt::ptr(self), fmt::ptr(v));
    if(rdstream_peek_i32be(self, v)) {
        self->position += sizeof(i32);
        return true;
    }
    return false;
}

bool rdstream_read_i64be(RDStream* self, i64* v) {
    spdlog::trace("rdstream_read_i64be({}, {})", fmt::ptr(self), fmt::ptr(v));
    if(rdstream_peek_i64be(self, v)) {
        self->position += sizeof(i64);
        return true;
    }
    return false;
}
