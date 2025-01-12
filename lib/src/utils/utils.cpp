#include "utils.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>

namespace redasm::utils {

namespace {

#ifdef _WIN32
constexpr char PATH_SEPARATOR = '\\';
#else
constexpr char PATH_SEPARATOR = '/';
#endif

} // namespace

namespace impl {

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

    if(res) *res = base;
}

} // namespace impl

std::string_view trim(std::string_view v) {
    int start = 0, end = v.size() - 1;

    while(start < static_cast<int>(v.size()) && std::isblank(v[start]))
        start++;

    while(end >= 0 && std::isblank(v[end]))
        end--;

    if(start < end) return v.substr(start, end - start + 1);

    return {};
}

tl::optional<Data> read_file(const std::string& filepath) {
    std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);
    if(!ifs.is_open()) return tl::nullopt;

    return Data(std::istreambuf_iterator<char>{ifs},
                std::istreambuf_iterator<char>{});
}

std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return s;
}

std::string_view get_filename(std::string_view p) {
    usize lastsep = p.find_last_of(PATH_SEPARATOR);
    if(lastsep != std::string_view::npos) return p.substr(lastsep + 1);
    return p;
}

std::string_view get_ext(std::string_view p) {
    usize lastdot = p.find_last_of('.');
    if(lastdot != std::string_view::npos) return p.substr(lastdot + 1);
    return p.substr(p.size());
}

} // namespace redasm::utils
