#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

struct RDMemoryInfo {
    union {
        RDAddress start;
        RDAddress baseaddress;
    };

    union {
        RDAddress end;
        RDAddress end_baseaddress;
    };

    usize size;
};

typedef struct RDStringResult {
    const char* type;
    const char* value;
    usize totalsize;
    bool terminated;
} RDStringResult;

REDASM_EXPORT void rd_memoryinfo(RDMemoryInfo* mi);
REDASM_EXPORT usize rd_memoryread(RDAddress address, char* data, usize n);
REDASM_EXPORT bool rd_map(RDAddress startaddr, RDAddress endaddr);
REDASM_EXPORT bool rd_map_n(RDAddress baseaddress, usize size);
