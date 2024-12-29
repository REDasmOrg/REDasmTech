#pragma once

#include <array>
#include <redasm/byteorder.h>
#include <redasm/types.h>

namespace redasm::byteorder {

constexpr bool is_littleendian() { return BO_PLATFORM == BO_LITTLE; }
constexpr bool is_bigendian() { return BO_PLATFORM == BO_BIG; }

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
