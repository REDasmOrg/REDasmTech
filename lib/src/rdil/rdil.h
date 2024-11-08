#pragma once

#include "../disasm/function.h"
#include "expression.h"
#include "expressionlist.h"
#include <redasm/rdil.h>
#include <redasm/renderer.h>
#include <string_view>

namespace redasm::rdil {

std::string_view get_op_name(usize t);
usize get_op_type(std::string_view id);
std::string get_text(const ILExpr* e);
std::string get_format(const ILExpr* e);

void generate(const Function& f, ILExprList& res);
void render(const ILExpr* e, const RDRendererParams& rp);

} // namespace redasm::rdil
