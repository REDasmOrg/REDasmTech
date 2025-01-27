#pragma once

#include "../../typing/typing.h"
#include <redasm/typing.h>

namespace redasm::api::internal {

usize size_of(std::string_view tname);
usize size_of(const RDType* t);
std::string type_name(RDType t);
bool create_type(std::string_view tname, RDType* t);
bool create_type_n(std::string_view tname, usize n, RDType* t);
bool int_from_bytes(usize b, bool sign, RDType* t);

} // namespace redasm::api::internal
