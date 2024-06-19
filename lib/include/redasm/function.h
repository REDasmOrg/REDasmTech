#pragma once

#include <redasm/common.h>
#include <redasm/graph.h>
#include <redasm/types.h>

RD_HANDLE(RDFunction);

typedef struct RDFunctionBasicBlock {
    RDAddress start;
    RDAddress end;
} RDFunctionBasicBlock;

REDASM_EXPORT RDFunction* rd_getfunction(RDAddress address);
REDASM_EXPORT RDGraph* rdfunction_getgraph(RDFunction* self);
REDASM_EXPORT bool rdfunction_contains(const RDFunction* self,
                                       RDAddress address);
REDASM_EXPORT bool rdfunction_getbasicblock(const RDFunction* self,
                                            RDGraphNode n,
                                            RDFunctionBasicBlock* bb);
