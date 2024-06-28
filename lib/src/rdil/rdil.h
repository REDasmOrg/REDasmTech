#pragma once

#include "expression.h"
#include <redasm/rdil.h>
#include <string_view>

namespace redasm::rdil {

std::string_view get_op_name(usize t);
usize get_op_type(std::string_view id);
std::string get_text(const Expression* e);
std::string get_format(const Expression* e);

} // namespace redasm::rdil
