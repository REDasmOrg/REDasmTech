#pragma once

#include <redasm/types.h>
#include <string>

namespace redasm {

struct Segment {
    std::string name;
    usize type;
    usize index;
    usize endindex;
    RDOffset offset;
    RDOffset endoffset;
};

} // namespace redasm
