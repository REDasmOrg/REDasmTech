#pragma once

#include <redasm/redasm.h>
#include <string_view>
#include <tl/optional.hpp>

namespace redasm::stringfinder {

tl::optional<RDStringResult> classify(usize idx);
bool is_gibberish(std::string_view s);

} // namespace redasm::stringfinder
