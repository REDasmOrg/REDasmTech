#include "stringfinder.h"
#include "../context.h"
#include "../memory/mbyte.h"
#include "../memory/memory.h"
#include "../state.h"
#include <cmath>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace redasm::stringfinder {

namespace {

constexpr float CHARS_FREQ = 0.7;
constexpr float ENTROPY_TRESHOLD = 3.5;
constexpr usize STR_MINUNIQUE = 2;
constexpr usize STR_MINLENGTH = 4;

std::string g_tempstr;

template<typename T>
bool is_ascii(T c) {
    return c != 0x0b && ((c >= 0x09 && c <= 0x0D) || (c >= 0x20 && c <= 0x7E));
}

bool check_formats(std::string_view s) {
    static const std::unordered_set<std::string_view> FORMATS = {
        "%c",   "%d", "%e",  "%E",  "%f",  "%g",  "%G",  "%hi",  "%hu",
        "%i",   "%l", "%ld", "%li", "%lf", "%Lf", "%lu", "%lli", "%lld",
        "%llu", "%o", "%p",  "%s",  "%u",  "%x",  "%X",  "%n",   "%%",
    };

    return FORMATS.find(s) != FORMATS.end();
}

double entropy(std::string_view str) {
    std::unordered_map<char, int> frequency;
    for(char ch : str)
        frequency[ch]++;

    double e = 0.0;
    auto len = static_cast<double>(str.size());

    for(const auto& [ch, c] : frequency) {
        double prob = c / len;
        e -= prob * std::log2(prob);
    }

    return e;
}

bool validate_string(std::string_view s) {
    if(s.empty()) return false;

    auto match_delimiters = [](char open, char close, std::string_view str) {
        return str.size() > 2 && str.front() == open && str.back() == close;
    };

    switch(s.front()) {
        case '\'': return match_delimiters('\'', '\'', s);
        case '\"': return match_delimiters('\"', '\"', s);
        case '<': return match_delimiters('<', '>', s);
        case '(': return match_delimiters('(', ')', s);
        case '[': return match_delimiters('[', ']', s);
        case '{': return match_delimiters('{', '}', s);
        case '%': return stringfinder::check_formats(s);
        default: break;
    }

    if(s.size() >= STR_MINLENGTH) return true;
    return !stringfinder::is_gibberish(s);
}

template<typename ToAsciiCallback>
std::pair<bool, RDStringResult>
categorize_as(const RDSegment* seg, RDAddress address, std::string_view tname,
              ToAsciiCallback cb) {
    g_tempstr.clear();
    char ch{};

    usize sz = state::context->types.size_of(tname);

    for(; address < seg->end; address += sz) {
        auto v = memory::get_type(seg, address, tname);
        if(!v) break;
        bool ok = cb(*v, ch);
        rdvalue_destroy(&v.value());
        if(!ok) break;
        g_tempstr.push_back(ch);
    }

    usize totalsize = g_tempstr.size() * sz;
    bool terminated = false;

    if(!ch) {
        terminated = true;
        totalsize += sz; // Include null terminator too
    }

    RDStringResult r = {
        {},
        g_tempstr.c_str(),
        totalsize,
        terminated,
    };

    return {stringfinder::validate_string(g_tempstr), r};
}

} // namespace

tl::optional<RDStringResult> classify(RDAddress address) {
    const RDSegment* seg = state::context->program.find_segment(address);
    if(!seg) return tl::nullopt;

    if(usize r = seg->end - address; r < sizeof(u16)) return tl::nullopt;

    auto mb1 = memory::get_mbyte(seg, address);
    auto mb2 = memory::get_mbyte(seg, address + 1);

    if(!mb1 || !mb2 || !mbyte::has_byte(*mb1) || !mbyte::has_byte(*mb2))
        return tl::nullopt;

    u8 b1 = mbyte::get_byte(*mb1);
    u8 b2 = mbyte::get_byte(*mb2);

    if(stringfinder::is_ascii(b1) && !b2) {
        auto [ok, c] = stringfinder::categorize_as(
            seg, address, "wchar", [](const RDValue& v, char& outch) {
                outch = v.ch_v;
                return stringfinder::is_ascii(v.ch_v);
            });

        if(ok) {
            if(c.terminated)
                c.type = state::context->types.create_primitive(T_WSTR);
            else {
                c.type = state::context->types.create_primitive(
                    T_WCHAR, g_tempstr.size());
            }
        }
        else
            return tl::nullopt;

        return c;
    }

    auto [ok, c] = stringfinder::categorize_as(
        seg, address, "char", [](const RDValue& v, char& outch) {
            outch = v.ch_v;
            return stringfinder::is_ascii(v.ch_v);
        });

    if(ok) {
        if(c.terminated)
            c.type = state::context->types.create_primitive(T_STR);
        else {
            c.type = state::context->types.create_primitive(T_CHAR,
                                                            g_tempstr.size());
        }
    }
    else
        return tl::nullopt;

    return c;
}

bool is_gibberish(std::string_view s) {
    // Step 1: Check for excessive repetition
    std::unordered_set<char> uniquechars{s.begin(), s.end()};
    if(uniquechars.size() <= STR_MINUNIQUE) // Too few unique characters
        return true;

    // Check if one character dominates the string (e.g., "aaaaaa")
    for(char c : uniquechars) {
        double freq =
            std::count(s.begin(), s.end(), c) / static_cast<double>(s.size());
        if(freq > CHARS_FREQ) // More than 70% of one character
            return true;
    }

    // Step 2: Calculate entropy
    double e = stringfinder::entropy(s);
    if(e < ENTROPY_TRESHOLD) // Low entropy indicates repetition
        return true;

    // Step 3: Check structure (no spaces in a long string)
    if(s.find(' ') == std::string_view::npos && s.size() > 10) return true;

    // If none of the conditions for gibberish are met, it's valid
    return false;
}

} // namespace redasm::stringfinder
