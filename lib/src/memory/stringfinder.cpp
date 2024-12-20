#include "stringfinder.h"
#include "../context.h"
#include "../state.h"
#include "../typing/base.h"
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

std::string g_temptype;
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
    if(s.empty())
        return false;

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

    // Count letters and digits only
    // usize c = std::count_if(s.begin(), s.end(), ::isalnum);

    if(s.size() >= STR_MINLENGTH)
        return true;

    return !stringfinder::is_gibberish(s);
}

template<typename ToAsciiCallback>
std::pair<bool, RDStringResult> categorize_as(usize idx, std::string_view tname,
                                              const ToAsciiCallback& cb) {

    const auto& mem = state::context->memory;
    g_tempstr.clear();
    char ch{};

    usize sz = state::context->types.size_of(tname);

    for(; idx < mem->size(); idx += sz) {
        auto v = mem->get_type(idx, tname);
        if(!v || !cb(std::forward<typing::Value>(*v), ch))
            break;

        g_tempstr.push_back(ch);
    }

    usize totalsize = g_tempstr.size() * sz;
    bool terminated = false;

    if(!ch) {
        terminated = true;
        totalsize += sz; // Include null terminator too
    }

    RDStringResult r = {
        nullptr,
        g_tempstr.c_str(),
        totalsize,
        terminated,
    };

    return {stringfinder::validate_string(g_tempstr), r};
}

} // namespace

tl::optional<RDStringResult> classify(usize idx) {
    const auto& mem = state::context->memory;

    if(idx >= mem->size())
        return tl::nullopt;

    if(usize r = mem->size() - idx; r < sizeof(u16))
        return tl::nullopt;

    Byte b1 = mem->at(idx);
    Byte b2 = mem->at(idx + 1);

    if(!b1.has_byte() || !b2.has_byte())
        return tl::nullopt;

    if(stringfinder::is_ascii(b1.byte()) && !b2.byte()) {
        auto [ok, c] = stringfinder::categorize_as(
            idx, "wchar", [](typing::Value&& v, char& outch) {
                outch = v.ch_v;
                return stringfinder::is_ascii(v.ch_v);
            });

        if(ok) {
            if(c.terminated)
                g_temptype = typing::names::WSTR;
            else {
                g_temptype = state::context->types.as_array(
                    typing::names::WCHAR, g_tempstr.size());
            }
        }
        else
            return tl::nullopt;

        c.type = g_temptype.c_str();
        return c;
    }

    auto [ok, c] = stringfinder::categorize_as(
        idx, "char", [](typing::Value&& v, char& outch) {
            outch = v.ch_v;
            return stringfinder::is_ascii(v.ch_v);
        });

    if(ok) {
        if(c.terminated)
            g_temptype = typing::names::STR;
        else {
            g_temptype = state::context->types.as_array(typing::names::CHAR,
                                                        g_tempstr.size());
        }
    }
    else
        return tl::nullopt;

    c.type = g_temptype.c_str();
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
    if(s.find(' ') == std::string_view::npos && s.size() > 10)
        return true;

    // If none of the conditions for gibberish are met, it's valid
    return false;
}

} // namespace redasm::stringfinder
