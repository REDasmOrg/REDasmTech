#pragma once

#include "byteorder.h"
#include <climits>
#include <redasm/buffer.h>
#include <redasm/typing.h>
#include <string>
#include <string_view>
#include <tl/optional.hpp>

namespace redasm::buffer {

tl::optional<u8> get_byte(const RDBuffer* self, usize idx);
tl::optional<RDMByte> get_mbyte(const RDBuffer* self, usize idx);

namespace impl {

template<typename T>
tl::optional<T> get_number(const RDBuffer* self, usize idx, bool big) {
    static constexpr usize N = sizeof(T);
    T num = 0;

    for(usize i = 0; i < N; i++) {
        if(auto b = buffer::get_byte(self, idx + i); b)
            num |= (static_cast<T>(*b) << (i * CHAR_BIT));
        else
            return tl::nullopt;
    }

    if constexpr(sizeof(T) == sizeof(u8))
        return num;
    else {
        if(big) return byteorder::to_bigendian(num);
        return byteorder::to_littleendian(num);
    }
}

} // namespace impl

usize read(const RDBuffer* self, usize idx, void* dst, usize n);
tl::optional<RDValue> read_struct_n(const RDBuffer* self, usize idx, usize n,
                                    const RDStructFieldDecl* fields);
tl::optional<RDValue> read_struct_n(const RDBuffer* self, usize idx, usize n,
                                    const RDStructFieldDecl* fields,
                                    usize& curridx);
tl::optional<RDValue> read_struct(const RDBuffer* self, usize idx,
                                  const RDStructFieldDecl* fields);
tl::optional<RDValue> read_struct(const RDBuffer* self, usize idx,
                                  const RDStructFieldDecl* fields,
                                  usize& curridx);
tl::optional<std::string> get_str(const RDBuffer* self, usize idx);
tl::optional<std::string> get_str(const RDBuffer* self, usize idx, usize n);
tl::optional<std::string> get_wstr(const RDBuffer* self, usize idx);
tl::optional<std::string> get_wstr(const RDBuffer* self, usize idx, usize n);
tl::optional<RDValue> get_type(const RDBuffer* self, usize idx,
                               std::string_view tn);
tl::optional<RDValue> get_type(const RDBuffer* self, usize idx,
                               std::string_view tn, usize& curridx);
tl::optional<RDValue> get_type(const RDBuffer* self, usize idx, RDType t);
tl::optional<RDValue> get_type(const RDBuffer* self, usize idx, RDType t,
                               usize& curridx);
tl::optional<bool> get_bool(const RDBuffer* self, usize idx);
tl::optional<char> get_char(const RDBuffer* self, usize idx);
tl::optional<char> get_wchar(const RDBuffer* self, usize idx);

inline tl::optional<u8> get_u8(const RDBuffer* self, usize idx) {
    return buffer::get_byte(self, idx);
}

inline tl::optional<u16> get_u16(const RDBuffer* self, usize idx, bool big) {
    return impl::get_number<u16>(self, idx, big);
}

inline tl::optional<u32> get_u32(const RDBuffer* self, usize idx, bool big) {
    return impl::get_number<u32>(self, idx, big);
}

inline tl::optional<u64> get_u64(const RDBuffer* self, usize idx, bool big) {
    return impl::get_number<u64>(self, idx, big);
}

inline tl::optional<i8> get_i8(const RDBuffer* self, usize idx) {
    if(auto b = buffer::get_u8(self, idx); b) return static_cast<i8>(*b);
    return tl::nullopt;
}

inline tl::optional<i16> get_i16(const RDBuffer* self, usize idx, bool big) {
    return impl::get_number<i16>(self, idx, big);
}

inline tl::optional<i32> get_i32(const RDBuffer* self, usize idx, bool big) {
    return impl::get_number<i32>(self, idx, big);
}

inline tl::optional<i64> get_i64(const RDBuffer* self, usize idx, bool big) {
    return impl::get_number<i64>(self, idx, big);
}

} // namespace redasm::buffer
