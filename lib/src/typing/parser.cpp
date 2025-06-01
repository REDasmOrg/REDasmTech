#include "parser.h"
#include "../utils/utils.h"
#include <cctype>
#include <redasm/ct.h>

namespace redasm::typing {

tl::optional<ParsedType> parse(std::string_view tstr) {
    if(tstr.empty()) return tl::nullopt;

    usize idx = 0;
    std::string_view strn;
    ParsedType pt{};

    // Ignore leading whitespaces
    while(idx < tstr.size() && std::isspace(tstr[idx]))
        idx++;

    if(std::isalpha(tstr.front()) || tstr.front() == '_') {
        while(idx < tstr.size() &&
              (std::isalnum(tstr[idx]) || tstr[idx] == '_'))
            idx++;
        pt.name = tstr.substr(0, idx);
    }

    if(pt.name.empty()) return tl::nullopt;

    if(idx < tstr.size() && tstr[idx] == '[') {
        usize startidx = idx + 1;
        while(idx < tstr.size() && tstr[idx] != ']')
            idx++;

        if(idx >= tstr.size()) return tl::nullopt;

        strn = tstr.substr(startidx, idx - startidx);
        idx++; // Skip ']'
    }

    if(!strn.empty()) {
        auto nval = utils::to_integer(strn);
        if(!nval || !nval.value()) return tl::nullopt;
        pt.n = *nval;
    }

    // Ignore trailing whitespaces and check for junk
    while(idx < tstr.size()) {
        if(std::isspace(idx++)) continue;
        return tl::nullopt;
    }

    return pt;
}

} // namespace redasm::typing
