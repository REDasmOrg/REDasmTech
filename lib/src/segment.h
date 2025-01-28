#pragma once

#include "types.h"

namespace redasm {

struct Segment {
    const char* name;
    MIndex index;
    MIndex endindex;
    RDOffset offset;
    RDOffset endoffset;
    u16 sel;
    u8 perm;
    u8 bits;
};

} // namespace redasm
