#include "pattern.h"
#include "../internal/buffer_internal.h"
#include "../memory/mbyte.h"
#include "utils.h"
#include <cctype>

namespace redasm::pattern {

Compiled compile(std::string_view psource) {
    Compiled cp;

    for(usize i = 0; i < psource.size();) {
        if(i + 1 >= psource.size()) {
            spdlog::error("Invalid pattern length");
            return {};
        }

        if(psource[i] == '?') {
            if(psource[i + 1] != '?') {
                spdlog::error("Invalid wildcard pattern");
                return {};
            }
            cp.emplace_back(tl::nullopt);
            i += 2;
        }
        else if(std::isxdigit(psource[i])) {
            auto b = utils::to_integer(psource.substr(i, 2), 16);
            if(!b) {
                spdlog::error("Invalid hex pattern");
                return {};
            }

            cp.emplace_back(b.value());
            i += 2;
        }
        else if(psource[i] == ' ')
            i++;
        else {
            spdlog::error("Invalid pattern");
            return {};
        }
    }

    return cp;
}

bool match_mbytes(const Compiled& pat, RDBuffer* b, usize idx) {
    if(pat.empty() || idx + pat.size() >= b->length) return false;

    for(const auto& p : pat) {
        if(p.has_value() != mbyte::has_byte(b->m_data[idx])) return false;
        if(p.has_value() && *p != mbyte::get_byte(b->m_data[idx])) return false;
        idx++;
    }

    return true;
}

} // namespace redasm::pattern
