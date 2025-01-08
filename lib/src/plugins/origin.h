#pragma once

#include "../error.h"
#include <fmt/format.h>
#include <string_view>

#define PM_RETURN_ORIGIN_CASE(x)                                               \
    case Origin::x: return #x

namespace redasm::pm {

// Supported programming languages
enum class Origin {
    NATIVE = 0,
    PYTHON,
};

inline std::string_view format_as(Origin f) {
    switch(f) {
        PM_RETURN_ORIGIN_CASE(NATIVE);
        PM_RETURN_ORIGIN_CASE(PYTHON);
        default: break;
    }

    unreachable;
}

} // namespace redasm::pm
