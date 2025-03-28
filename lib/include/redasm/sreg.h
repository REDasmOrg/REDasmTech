#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

typedef struct RDSRegRange {
    int sreg;
    RDAddress start;
    RDAddress end;
    u64 value;
    HListNode hnode;
} RDSRegRange;

define_hmap(RDSRegRangeHMap, 1 << 4);

REDASM_EXPORT u64 rd_getsreg_ex(RDAddress address, int sreg, u64 d);
REDASM_EXPORT bool rd_getsreg(RDAddress address, int sreg, u64* val);
REDASM_EXPORT void rd_addsreg_range(RDAddress start, RDAddress end, int sreg,
                                    u64 val);
REDASM_EXPORT void rd_setsreg(RDAddress address, int sreg, u64 val);
REDASM_EXPORT void rd_setsreg_from(RDAddress address, int sreg, u64 val,
                                   RDAddress fromaddr);
