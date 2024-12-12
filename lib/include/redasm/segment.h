#pragma once

#include <redasm/types.h>

typedef enum RDSegmentType {
    SEG_UNKNOWN = 0,
    SEG_HASDATA = 1 << 0,
    SEG_HASCODE = 1 << 1,
} RDSegmentType;

typedef struct RDSegment {
    const char* name;
    usize type;
    RDAddress address;
    RDAddress endaddress;
    RDOffset offset;
    RDOffset endoffset;
} RDSegment;
