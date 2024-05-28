#include "utils.h"
#include <algorithm>
#include <cctype>
#include <charconv>
#include <fstream>
#include <iterator>

namespace redasm::utils {

namespace {

void detect_base(std::string_view& sv, int* res) {
    int base = 10;

    if(sv.size() > 2 && sv[0] == '0') {
        switch(sv[1]) {
            case 'x': base = 16; break;
            case 'o': base = 8; break;
            case 'b': base = 2; break;
            default: break;
        }

        sv = sv.substr(2);
    }

    if(res)
        *res = base;
}

} // namespace

tl::optional<usize> to_integer(std::string_view sv, int base) {
    if(sv.empty())
        return tl::nullopt;

    utils::detect_base(sv, !base ? &base : nullptr);

    size_t val{};
    auto res = std::from_chars(sv.begin(), sv.end(), val, base);
    if(res.ec != std::errc{} || res.ptr < sv.end())
        return tl::nullopt;
    return val;
}

tl::optional<Data> read_file(const std::string& filepath) {
    std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);
    if(!ifs.is_open())
        return tl::nullopt;

    return Data(std::istreambuf_iterator<char>{ifs},
                std::istreambuf_iterator<char>{});
}

std::string_view trim(std::string_view v) {
    int start = 0, end = v.size() - 1;

    while(start < static_cast<int>(v.size()) && std::isblank(v[start]))
        start++;

    while(end >= 0 && std::isblank(v[end]))
        end--;

    if(start < end)
        return v.substr(start, end - start + 1);

    return std::string_view{};
}

std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return s;
}

} // namespace redasm::utils
