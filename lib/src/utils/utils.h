#pragma once

#include "../error.h"
#include <charconv>
#include <redasm/types.h>
#include <string>
#include <string_view>
#include <tl/optional.hpp>
#include <vector>

namespace redasm::utils {

namespace impl {

void detect_base(std::string_view& sv, int& res);

} // namespace impl

template<typename>
inline constexpr bool AlwaysFalseV = false; // NOLINT

using Data = std::vector<u8>;

usize count_bits(i64 n);
char* copy_str(std::string_view v);
std::string_view trim(std::string_view v);

template<typename Ret = std::string_view, typename T>
Ret to_string(T value, int base = 10, bool sign = false, int bits = 0) {
    constexpr std::string_view DIGITS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static std::array<char, 66> out;

    if(base < 2 || base > 36) except("tostring: invalid base {}", base);

    bool isneg = false;

    if(sign && value < 0) {
        isneg = true;
        value = -value;
    }

    int c = out.size() - 1;
    out[c] = 0;

    do {
        T rem = value % base;
        value /= base;
        out[--c] = DIGITS.at(rem);
    } while(value > 0);

    if(isneg) out[--c] = '-';

    if(bits > 0) { // Zero pad
        int w = bits / 4;
        if(int ndigits = out.size() - c - 1; w > ndigits) {
            int nzeros = w - ndigits;
            for(int i = c - 1; i >= c - nzeros && i >= 0; --i)
                out[i] = '0';

            c -= nzeros;
        }
    }

    return Ret{&out[c], out.size() - c - 1};
}

template<typename Ret = std::string_view, typename T>
static Ret to_hex(T value, int bits = 0) {
    if(!bits) bits = sizeof(T) * CHAR_BIT;
    return utils::to_string<Ret, T>(value, 16, false, bits);
}

template<typename T = usize>
[[nodiscard]] tl::optional<T> to_integer(std::string_view sv, int base = 0) {
    if(sv.empty()) return tl::nullopt;
    if(!base) impl::detect_base(sv, base);

    T val{};
    auto res = std::from_chars(sv.begin(), sv.end(), val, base);
    if(res.ec != std::errc{} || res.ptr < sv.end()) return tl::nullopt;
    return val;
}

template<typename Function>
void split_each(std::string_view s, char sep, Function f) {
    usize i = 0, start = 0;

    for(; i < s.size(); i++) {
        if(s[i] != sep) continue;
        if(i > start && !f(utils::trim(s.substr(start, i - start)))) return;
        start = i + 1;
    }

    if(start < s.size()) f(utils::trim(s.substr(start)));
}

tl::optional<Data> read_file(const std::string& filepath);
std::string to_lower(std::string s);
std::string_view get_filename(std::string_view p);
std::string_view get_ext(std::string_view p);

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
