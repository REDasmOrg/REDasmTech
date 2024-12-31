#pragma once

#include "../../typing/typing.h"
#include <redasm/typing.h>

namespace redasm::api::internal {

usize size_of(std::string_view tname);
typing::Value* create_value();
std::string type_name(RDType t);
bool create_type(std::string_view tname, RDType* t);
bool create_type_n(std::string_view tname, usize n, RDType* t);
bool int_from_bytes(usize b, bool sign, RDType* t);
std::string create_struct(const std::string& name,
                          const typing::StructBody& fields);

} // namespace redasm::api::internal
