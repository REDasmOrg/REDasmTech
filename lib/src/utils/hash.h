#pragma once

#include <redasm/types.h>
#include <string>
#include <string_view>

namespace redasm::hash {

namespace impl {

static constexpr u32 MURMUR2_SEED = 0xbc9f1d34;

constexpr u32 get_u32(const char* p) {
    return static_cast<u8>(p[0]) << 0 | static_cast<u8>(p[1]) << 8 |
           static_cast<u8>(p[2]) << 16 | static_cast<u8>(p[3]) << 24;
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

// https://github.com/aappleby/smhasher/blob/master/src/MurmurHash2.cpp
constexpr u32 murmur2(const char* key, usize len) {
    constexpr u32 M = 0x5bd1e995;
    constexpr int R = 24;

    u32 h = impl::MURMUR2_SEED ^ len;
    const char* data = key;

    while(len >= 4) {
        u32 k = impl::get_u32(data);

        k *= M;
        k ^= k >> R;
        k *= M;

        h *= M;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch(len) {
        case 3: h ^= data[2] << 16; [[fallthrough]];

        case 2: h ^= data[1] << 8; [[fallthrough]];

        case 1:
            h ^= data[0];
            h *= M;
            [[fallthrough]];

        default: break;
    }

    h ^= h >> 13;
    h *= M;
    h ^= h >> 15;
    return h;
}

constexpr u32 murmur2(std::string_view arg) {
    return hash::murmur2(arg.data(), arg.size());
}
static inline u32 murmur2(const std::string& arg) {
    return hash::murmur2(arg.data(), arg.size());
}

} // namespace redasm::hash
