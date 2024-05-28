#pragma once

#include <redasm/types.h>
#include "../../typing/typing.h"
#include <string_view>
#include <tl/optional.hpp>

namespace redasm::api::internal {

usize file_size();
tl::optional<typing::Value> file_map_type(RDOffset offset,
                                          std::string_view tname);

tl::optional<typing::Value> file_map_type_as(RDOffset offset,
                                             std::string_view tname,
                                             const std::string& dbname);

} // namespace redasm::api::internal
