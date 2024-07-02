#pragma once

#include "../disasm/function.h"
#include "expression.h"
#include "function.h"
#include <redasm/rdil.h>
#include <string_view>

namespace redasm::rdil {

std::string_view get_op_name(usize t);
usize get_op_type(std::string_view id);
std::string get_text(const ILExpression* e);
std::string get_format(const ILExpression* e);

bool generate(const Function& f, ILFunction& ilf);

} // namespace redasm::rdil
