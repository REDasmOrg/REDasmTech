#pragma once

#include <algorithm>
#include <redasm/buffer.h>
#include <redasm/types.h>
#include <string_view>
#include <tl/optional.hpp>
#include <vector>

namespace redasm::pattern {

using Compiled = std::vector<tl::optional<u8>>;

Compiled compile(std::string_view psource);
bool match_mbytes(const Compiled& pat, RDBuffer* b, usize idx);

inline void sort(std::vector<Compiled>& patterns) {
    std::ranges::sort(patterns, [](const auto& lhs, const auto& rhs) {
        return lhs.size() > rhs.size();
    });
}

inline std::vector<Compiled> compile_all(const char** psource) {
    ct_assume(psource);

    std::vector<pattern::Compiled> patterns;
    for(const char** p = psource; *p; p++)
        patterns.push_back(pattern::compile(*p));
    pattern::sort(patterns);
    return patterns;
}

} // namespace redasm::pattern
