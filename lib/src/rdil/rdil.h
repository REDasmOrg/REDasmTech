#pragma once

#include "../disasm/function.h"
#include "expression.h"
#include "expressionlist.h"
#include <redasm/rdil.h>
#include <redasm/renderer.h>
#include <string_view>

namespace redasm {

class Renderer;

namespace rdil {

std::string_view get_op_name(usize t);
usize get_op_type(std::string_view id);
std::string get_text(const ILExpr* e);
std::string get_format(const ILExpr* e);

void generate(const Function& f, ILExprList& res);
void generate(const Function& f, ILExprList& res, usize maxn);
void render(const ILExpr* e, Renderer& render);

} // namespace rdil

} // namespace redasm
