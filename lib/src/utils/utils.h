#pragma once

#include <charconv>
#include <redasm/types.h>
#include <string>
#include <string_view>
#include <tl/optional.hpp>
#include <vector>

namespace redasm::utils {

namespace impl {

void detect_base(std::string_view& sv, int* res);

} // namespace impl

template<typename>
inline constexpr bool AlwaysFalseV = false; // NOLINT

using Data = std::vector<u8>;

template<typename T = usize>
[[nodiscard]] tl::optional<T> to_integer(std::string_view sv, int base = 0) {
    if(sv.empty())
        return tl::nullopt;

    impl::detect_base(sv, !base ? &base : nullptr);

    T val{};
    auto res = std::from_chars(sv.begin(), sv.end(), val, base);

    if(res.ec != std::errc{} || res.ptr < sv.end())
        return tl::nullopt;

    return val;
}

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
