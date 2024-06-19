#pragma once

#include <redasm/function.h>

namespace redasm::api::internal {

RDFunction* get_function(RDAddress address);
RDGraph* function_getgraph(RDFunction* self);
bool function_contains(const RDFunction* self, RDAddress address);
bool function_getbasicblock(const RDFunction* self, RDGraphNode n,
                            RDFunctionBasicBlock* bb);

} // namespace redasm::api::internal
