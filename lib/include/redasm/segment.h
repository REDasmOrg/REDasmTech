#pragma once

#include <redasm/buffer.h>
#include <redasm/types.h>

typedef enum RDSegmentPerm {
    SP_R = 1 << 0,
    SP_W = 1 << 1,
    SP_X = 1 << 2,

    SP_RW = SP_R | SP_W,
    SP_RX = SP_R | SP_X,
    SP_RWX = SP_R | SP_W | SP_X,
    SP_WX = SP_W | SP_X,
} RDSegmentPerm;

typedef struct RDSegment {
    const char* name;
    RDAddress start;
    RDAddress end;
    u32 perm;
    u32 bits;
    RDBuffer* mem;
} RDSegment;

define_slice(RDSegmentSlice, RDSegment);
