#pragma once

#include "../../typing/typing.h"
#include <redasm/types.h>
#include <tl/optional.hpp>

namespace redasm::api::internal {

usize file_size();
tl::optional<typing::Value> file_map_type(RDOffset offset,
                                          typing::FullTypeName tname);

tl::optional<typing::Value> file_map_type_as(RDOffset offset,
                                             typing::FullTypeName tname,
                                             const std::string& dbname);

} // namespace redasm::api::internal
