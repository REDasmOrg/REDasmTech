#pragma once

#include <redasm/buffer.h>
#include <redasm/byte.h>
#include <redasm/types.h>

typedef enum RDSegmentPerm {
    SP_R = 1 << 0,
    SP_W = 1 << 1,
    SP_X = 1 << 2,

    SP_RW = SP_R | SP_W,
    SP_RX = SP_R | SP_X,
    SP_RWX = SP_R | SP_W | SP_X,
    SP_WX = SP_W | SP_X,

    SF_BSS = 1 << 7,
} RDSegmentPerm;

typedef struct RDSegment {
    const char* name;
    usize perm;
    RDAddress startaddr;
    RDAddress endaddr;
    RDOffset startoff;
    RDOffset endoff;
} RDSegment;

typedef struct RDSegmentNew {
    const char* name;
    RDAddress start;
    RDAddress end;
    u32 perm;
    u32 bits;
    RDBuffer mem;
} RDSegmentNew;

// typedef struct RDSegment {
//     const char* name;
//     RDAddress startaddr;
//     RDAddress endaddr;
//     RDOffset fileoff;
//     u16 sel;
//     u8 perm;
//     u8 bits;
// } RDSegment;
