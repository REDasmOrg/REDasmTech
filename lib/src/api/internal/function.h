#pragma once

#include <redasm/function.h>

namespace redasm::api::internal {

RDFunction* get_function(RDAddress address);
bool function_contains(const RDFunction* self, RDAddress address);
RDAddress function_getentry(const RDFunction* self);
RDThemeKind function_gettheme(const RDFunction* self, const RDGraphEdge* e);
bool function_isexport(const RDFunction* self);
RDGraph* function_getgraph(RDFunction* self);
bool function_getbasicblock(const RDFunction* self, RDGraphNode n,
                            RDFunctionBasicBlock* bb);

} // namespace redasm::api::internal
