#include "../internal/function.h"
#include <redasm/function.h>

RDFunction* rd_getfunction(RDAddress address) {
    return redasm::api::internal::get_function(address);
}

RDAddress rdfunction_getentry(const RDFunction* self) {
    return redasm::api::internal::function_getentry(self);
}

RDGraph* rdfunction_getgraph(RDFunction* self) {
    return redasm::api::internal::function_getgraph(self);
}

bool rdfunction_contains(const RDFunction* self, RDAddress address) {
    return redasm::api::internal::function_contains(self, address);
}

bool rdfunction_getbasicblock(const RDFunction* self, RDGraphNode n,
                              RDFunctionBasicBlock* bb) {
    return redasm::api::internal::function_getbasicblock(self, n, bb);
}
