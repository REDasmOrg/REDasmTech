#pragma once

#include <redasm/types.h>

typedef enum RDSegmentType {
    SEG_UNKNOWN = 0,
    SEG_HASDATA = 1 << 0,
    SEG_HASCODE = 1 << 1,
} RDSegmentType;

// typedef enum RDSegmentPerm {
//     SP_R = 1 << 0,
//     SP_W = 1 << 1,
//     SP_X = 1 << 2,
//
//     SP_RW = SP_R | SP_W,
//     SP_RX = SP_R | SP_X,
//     SP_RWX = SP_R | SP_W | SP_X,
//     SP_WX = SP_W | SP_X,
// } RDSegmentPerm;

typedef struct RDSegment {
    const char* name;
    usize type;
    RDAddress startaddr;
    RDAddress endaddr;
    RDOffset startoff;
    RDOffset endoff;
} RDSegment;

// typedef struct RDSegment {
//     const char* name;
//     RDAddress startaddr;
//     RDAddress endaddr;
//     RDOffset off;
//     u16 sel;
//     u8 perm;
//     u8 bits;
// } RDSegment;
