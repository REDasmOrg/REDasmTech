#pragma once

#include <redasm/common.h>
#include <redasm/graph.h>
#include <redasm/theme.h>
#include <redasm/types.h>

RD_HANDLE(RDFunction);

typedef struct RDBasicBlock {
    RDAddress start;
    RDAddress end;
} RDBasicBlock;

// clang-format off
REDASM_EXPORT RDFunction* rd_findfunction(RDAddress address);
REDASM_EXPORT RDAddress rdfunction_getentry(const RDFunction* self);
REDASM_EXPORT RDGraph* rdfunction_getgraph(RDFunction* self);
REDASM_EXPORT RDThemeKind rdfunction_gettheme(const RDFunction* self, const RDGraphEdge* edge);
REDASM_EXPORT bool rdfunction_isexport(const RDFunction* self);
REDASM_EXPORT bool rdfunction_contains(const RDFunction* self, RDAddress address);
REDASM_EXPORT const RDBasicBlock* rdfunction_getbasicblock(const RDFunction* self, RDGraphNode n);
// clang-format on
