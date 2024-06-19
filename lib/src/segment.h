#pragma once

#include "types.h"
#include <string>

namespace redasm {

struct Segment {
    std::string name;
    usize type;
    MIndex index;
    MIndex endindex;
    RDOffset offset;
    RDOffset endoffset;
};

} // namespace redasm
