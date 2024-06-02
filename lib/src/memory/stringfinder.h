#pragma once

#include <redasm/redasm.h>
#include <tl/optional.hpp>

namespace redasm::stringfinder {

tl::optional<RDStringResult> classify(usize idx);

}
