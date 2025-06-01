#pragma once

#include <redasm/redasm.h>
#include <string_view>
#include <tl/optional.hpp>

namespace redasm::typing {

struct ParsedType {
    std::string_view name;
    usize n;
};

tl::optional<ParsedType> parse(std::string_view tstr);

} // namespace redasm::typing
