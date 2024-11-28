#pragma once

// https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp
// https://github.com/AntonJohansson/StaticMurmur/blob/master/StaticMurmur.hpp

#include <redasm/types.h>
#include <string>
#include <string_view>

namespace redasm::hash {

namespace impl {

constexpr u32 MURMUR3_SEED = 0xbc9f1d34;

constexpr u32 get_block(const char* p, unsigned i) {
    return static_cast<u32>(p[0 + (i * 4)]) << 0 |
           static_cast<u32>(p[1 + (i * 4)]) << 8 |
           static_cast<u32>(p[2 + (i * 4)]) << 16 |
           static_cast<u32>(p[3 + (i * 4)]) << 24;
}

constexpr u32 rotl32(u32 x, u8 r) { return (x << r) | (x >> (32U - r)); }

constexpr u32 fmix32(u32 h) {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

} // namespace impl

struct StringHash {
    using is_transparent = void;

    [[nodiscard]] size_t operator()(const char* txt) const {
        return std::hash<std::string_view>{}(txt);
    }

    [[nodiscard]] size_t operator()(std::string_view txt) const {
        return std::hash<std::string_view>{}(txt);
    }

    [[nodiscard]] size_t operator()(const std::string& txt) const {
        return std::hash<std::string>{}(txt);
    }
};

constexpr u32 murmur3(const char* key, usize len) {
    constexpr u32 C1 = 0xcc9e2d51;
    constexpr u32 C2 = 0x1b873593;
    const auto NBLOCKS = static_cast<usize>(len / 4);
    u32 h1 = impl::MURMUR3_SEED;

    for(usize i = 0; i < NBLOCKS; i++) {
        u32 k1 = impl::get_block(key, i);

        k1 *= C1;
        k1 = impl::rotl32(k1, 15);
        k1 *= C2;

        h1 ^= k1;
        h1 = impl::rotl32(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }

    const usize TAIL = len - (len % 4);
    u32 k1 = 0;

    switch(len & 3) {
        case 3: k1 ^= key[TAIL + 2] << 16; [[fallthrough]];
        case 2: k1 ^= key[TAIL + 1] << 8; [[fallthrough]];

        case 1:
            k1 ^= key[TAIL + 0];
            k1 *= C1;
            k1 = impl::rotl32(k1, 15);
            k1 *= C2;
            h1 ^= k1;
            [[fallthrough]];

        default: break;
    };

    return impl::fmix32(h1 ^ len);
}

constexpr u32 static_murmur3(std::string_view arg) {
    return hash::murmur3(arg.data(), arg.size());
}

inline u32 murmur3(std::string_view arg) {
    return hash::murmur3(arg.data(), arg.size());
}

inline u32 murmur3(const std::string& arg) {
    return hash::murmur3(arg.data(), arg.size());
}

} // namespace redasm::hash
