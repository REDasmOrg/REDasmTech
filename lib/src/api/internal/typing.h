#pragma once

#include "../../typing/typing.h"
#include <redasm/typing.h>

namespace redasm::api::internal {

usize size_of(std::string_view tname);
bool create_type(std::string_view tname, RDType* t);
std::string create_struct(const std::string& name,
                          const typing::StructBody& fields);

} // namespace redasm::api::internal
