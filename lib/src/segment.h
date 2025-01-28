#pragma once

#include "types.h"

namespace redasm {

struct Segment {
    const char* name;
    usize perm;
    MIndex index;
    MIndex endindex;
    RDOffset offset;
    RDOffset endoffset;
};

} // namespace redasm
