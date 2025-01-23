#pragma once

#include "../segment.h"
#include "abstractbuffer.h"
#include "memory.h"
#include <memory>
#include <vector>

namespace redasm {

struct Program {
    ~Program();

    std::vector<Segment> segments;
    std::unique_ptr<Memory> memory;
    std::shared_ptr<AbstractBuffer> file;
};

} // namespace redasm
