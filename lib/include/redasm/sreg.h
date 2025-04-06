#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

typedef struct RDSRange {
    RDAddress start;
    RDAddress end;
    u64 value;
    RBTreeNode rbnode;
} RDSRange;

typedef struct RDSRegTree {
    int sreg;
    RBTree root;
    HListNode hnode;
} RDSRegTree;

define_hmap(RDSRangeMap, 1 << 4);

REDASM_EXPORT u64 rd_getsreg_ex(RDAddress address, int sreg, u64 d);
REDASM_EXPORT bool rd_getsreg(RDAddress address, int sreg, u64* val);
REDASM_EXPORT void rd_addsreg_range(RDAddress start, RDAddress end, int sreg,
                                    u64 val);
REDASM_EXPORT void rd_setsreg(RDAddress address, int sreg, u64 val);
REDASM_EXPORT void rd_setsreg_from(RDAddress address, int sreg, u64 val,
                                   RDAddress fromaddr);
