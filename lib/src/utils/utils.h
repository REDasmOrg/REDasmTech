#pragma once

#include <redasm/types.h>
#include <string>
#include <string_view>
#include <tl/optional.hpp>
#include <vector>

namespace redasm::utils {

template<typename>
inline constexpr bool AlwaysFalseV = false; // NOLINT

using Data = std::vector<u8>;

[[nodiscard]] tl::optional<usize> to_integer(std::string_view sv, int base = 0);
tl::optional<Data> read_file(const std::string& filepath);
std::string_view trim(std::string_view v);
std::string to_lower(std::string s);

constexpr u32 fnv1a_32(std::string_view s) {
    u32 h = 2166136261U;

    for(char c : s) {
        h ^= c;
        h *= 16777619U;
    }

    return h;
}

namespace string_literals {

constexpr u32 operator"" _fnv1a_32(const char* s, usize size) {
    return utils::fnv1a_32(std::string_view{s, size});
}

static_assert("0123456789ABCDEF"_fnv1a_32 == 0xe9cb8efd);

} // namespace string_literals

} // namespace redasm::utils
