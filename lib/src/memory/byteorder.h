#pragma once

#include <redasm/types.h>
#include <array>
#include <sys/param.h>

#if defined(__BYTE_ORDER)
#if defined(__BIG_ENDIAN) && (__BYTE_ORDER == __BIG_ENDIAN)
#define REDASM_BIG_ENDIAN
#elif defined(__LITTLE_ENDIAN) && (__BYTE_ORDER == __LITTLE_ENDIAN)
#define REDASM_LITTLE_ENDIAN
#else
static_assert(false, "ByteOrder: Unsupported endianness")
#endif
#else
static_assert(false, "ByteOrder: Cannot detect byte order")
#endif

namespace redasm::byteorder {

enum class Endian : usize {
    LITTLE = 0,
    BIG,

#if defined(REDASM_BIG_ENDIAN)
    PLATFORM = big,
#else
        PLATFORM = LITTLE
#endif
};

constexpr bool is_littleendian() { return Endian::PLATFORM == Endian::LITTLE; }
constexpr bool is_bigendian() { return Endian::PLATFORM == Endian::BIG; }

template<typename T>
T swap(T hostval) noexcept {
    union {
        T u;
        std::array<u8, sizeof(T)> b;
    } source, dest;

    source.u = hostval;

    for(usize i = 0; i < sizeof(T); i++)
        dest.b[i] = source.b[sizeof(T) - i - 1];

    return dest.u;
}

template<typename T>
T to_littleendian(T hostval) {
    if constexpr(byteorder::is_bigendian())
        return byteorder::swap<T>(hostval);
    return hostval;
}

template<typename T>
T to_bigendian(T hostval) {
    if constexpr(byteorder::is_littleendian())
        return byteorder::swap<T>(hostval);
    return hostval;
}

template<typename T>
T from_littleendian(T hostval) {
    if constexpr(byteorder::is_bigendian())
        return byteorder::swap<T>(hostval);
    return hostval;
}

template<typename T>
T from_bigendian(T hostval) {
    if constexpr(byteorder::is_littleendian())
        return byteorder::swap<T>(hostval);
    return hostval;
}

} // namespace redasm::byteorder
