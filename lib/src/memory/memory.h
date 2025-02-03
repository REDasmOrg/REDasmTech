#pragma once

#include "buffer.h"
#include <redasm/segment.h>
#include <tl/optional.hpp>

namespace redasm::memory {

// Memory Buffer interface
inline usize read(const RDSegment* self, RDAddress address, void* dst,
                  usize n) {
    return buffer::read(&self->mem, address - self->start, dst, n);
}

inline tl::optional<RDValue> read_struct(const RDSegment* self,
                                         RDAddress address,
                                         const RDStructField* fields) {
    return buffer::read_struct(&self->mem, address - self->start, fields);
}

inline tl::optional<u8> get_byte(const RDSegment* self, RDAddress address) {
    return buffer::get_byte(&self->mem, address - self->start);
}

inline tl::optional<bool> get_bool(const RDSegment* self, RDAddress address) {
    return buffer::get_bool(&self->mem, address - self->start);
}

inline tl::optional<char> get_char(const RDSegment* self, RDAddress address) {
    return buffer::get_char(&self->mem, address - self->start);
}

inline tl::optional<char> get_wchar(const RDSegment* self, RDAddress address) {
    return buffer::get_wchar(&self->mem, address - self->start);
}

inline tl::optional<u8> get_u8(const RDSegment* self, RDAddress address) {
    return buffer::get_byte(&self->mem, address - self->start);
}

inline tl::optional<u16> get_u16(const RDSegment* self, RDAddress address,
                                 bool big) {
    return buffer::get_u16(&self->mem, address - self->start, big);
}

inline tl::optional<u32> get_u32(const RDSegment* self, RDAddress address,
                                 bool big) {
    return buffer::get_u32(&self->mem, address - self->start, big);
}

inline tl::optional<u64> get_u64(const RDSegment* self, RDAddress address,
                                 bool big) {
    return buffer::get_u64(&self->mem, address - self->start, big);
}

inline tl::optional<i8> get_i8(const RDSegment* self, RDAddress address) {
    return buffer::get_i8(&self->mem, address - self->start);
}

inline tl::optional<i16> get_i16(const RDSegment* self, RDAddress address,
                                 bool big) {
    return buffer::get_i16(&self->mem, address - self->start, big);
}

inline tl::optional<i32> get_i32(const RDSegment* self, RDAddress address,
                                 bool big) {
    return buffer::get_i32(&self->mem, address - self->start, big);
}

inline tl::optional<i64> get_i64(const RDSegment* self, RDAddress address,
                                 bool big) {
    return buffer::get_i64(&self->mem, address - self->start, big);
}

inline tl::optional<std::string> get_str(const RDSegment* self,
                                         RDAddress address) {
    return buffer::get_str(&self->mem, address - self->start);
}

inline tl::optional<std::string> get_str(const RDSegment* self,
                                         RDAddress address, usize n) {
    return buffer::get_str(&self->mem, address - self->start, n);
}

inline tl::optional<std::string> get_wstr(const RDSegment* self,
                                          RDAddress address) {
    return buffer::get_wstr(&self->mem, address - self->start);
}

inline tl::optional<std::string> get_wstr(const RDSegment* self,
                                          RDAddress address, usize n) {
    return buffer::get_wstr(&self->mem, address - self->start, n);
}

inline tl::optional<RDValue> get_type(const RDSegment* self, RDAddress address,
                                      typing::FullTypeName tn) {
    return buffer::get_type(&self->mem, address - self->start, tn);
}

inline tl::optional<RDValue> get_type(const RDSegment* self, RDAddress address,
                                      RDType t) {
    return buffer::get_type(&self->mem, address - self->start, t);
}

// Memory interface
template<typename Function>
bool range_is(const RDSegment* self, RDAddress address, usize n, Function f) {
    RDAddress endaddr = std::min(address + n, self->end);

    for(RDAddress i = address; i < endaddr; i++) {
        auto b = memory::get_byte(self, address);
        if(!b || !f(*b)) return false;
    }

    return true;
}

usize get_length(const RDSegment* self, RDAddress address);
tl::optional<RDAddress> get_next(const RDSegment* self, RDAddress address);
bool is_unknown(const RDSegment* self, RDAddress address);
bool has_common(const RDSegment* self, RDAddress address);
bool has_byte(const RDSegment* self, RDAddress address);
bool has_flag(const RDSegment* self, RDAddress address, u32 f);
void set_flag(RDSegment* self, RDAddress address, u32 f, bool b = true);
void clear(RDSegment* self, RDAddress address);
void set_n(RDSegment* self, RDAddress address, usize n, u32 flags);
void unset_n(RDSegment* self, RDAddress address, usize n);
RDMByte get_mbyte(const RDSegment* self, RDAddress address);

} // namespace redasm::memory
