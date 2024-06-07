#include "stringfinder.h"
#include "../context.h"
#include "../state.h"
#include <algorithm>
#include <string_view>

namespace redasm::stringfinder {

namespace {

constexpr float ALPHA_THRESHOLD = 0.5;
constexpr usize DS_MINLENGTH = 4;
constexpr usize CS_MINLENGTH = 16;

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

bool check_heuristic(std::string_view s, bool gibberish) {
    if(s.empty())
        return false;

    switch(s.front()) {
        case '\'':
            if(s.size() < 2 || s.back() != '\'')
                break;
            [[fallthrough]];

        case '\"':
            if(s.size() < 2 || s.back() != '\"')
                break;
            [[fallthrough]];

        case '<':
            if(s.size() < 2 || s.back() != '>')
                break;
            [[fallthrough]];

        case '(':
            if(s.size() < 2 || s.back() != ')')
                break;
            [[fallthrough]];

        case '[':
            if(s.size() < 2 || s.back() != ']')
                break;
            [[fallthrough]];

        case '{':
            if(s.size() < 2 || s.back() != '}')
                break;
            [[fallthrough]];

        case '%': return stringfinder::check_formats(s);
        default: return gibberish; // TODO(davide): Add Gibberish analyzer
    }

    return false;
}

bool validate_string(std::string_view s) {
    if(!stringfinder::check_heuristic(s, true))
        return false;

    auto alnumcount =
        static_cast<double>(std::count_if(s.begin(), s.end(), ::isalnum));
    return (alnumcount / static_cast<double>(s.size())) > ALPHA_THRESHOLD;
}

template<typename ToAsciiCallback>
std::pair<bool, RDStringResult> categorize_as(usize idx, std::string_view tname,
                                              const ToAsciiCallback& cb) {

    const Memory* mem = state::context->memory.get();
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

    const Segment* seg = state::context->index_to_segment(idx);

    if(seg) {
        if(seg->type & SEGMENTTYPE_HASDATA && g_tempstr.size() > DS_MINLENGTH)
            return {stringfinder::validate_string(g_tempstr), r};
        if(seg->type & SEGMENTTYPE_HASCODE && g_tempstr.size() > CS_MINLENGTH)
            return {stringfinder::validate_string(g_tempstr), r};
    }

    return {stringfinder::check_heuristic(g_tempstr, false), r};
}

} // namespace

tl::optional<RDStringResult> classify(usize idx) {
    const Memory* mem = state::context->memory.get();

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

} // namespace redasm::stringfinder
