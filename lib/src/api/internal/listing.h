#pragma once

#include <redasm/listing.h>
#include <redasm/types.h>
#include <tl/optional.hpp>

namespace redasm::api::internal {

bool listing_getsymbol(usize idx, RDSymbol* symbol);
usize listing_getsymbolslength();
bool listing_getimport(usize idx, RDSymbol* symbol);
usize listing_getimportslength();
bool listing_getexport(usize idx, RDSymbol* symbol);
usize listing_getexportslength();

tl::optional<usize> listing_getindex(RDAddress address);
usize listing_getlength();

} // namespace redasm::api::internal
