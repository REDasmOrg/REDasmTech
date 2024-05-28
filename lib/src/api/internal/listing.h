#pragma once

#include <redasm/listing.h>
#include <redasm/types.h>
#include <tl/optional.hpp>

namespace redasm::api::internal {

bool listing_getsymbol(usize idx, RDSymbol* symbol);
tl::optional<usize> listing_getindex(RDAddress address);
usize listing_getsymbolslength();
usize listing_getlength();

} // namespace redasm::api::internal
