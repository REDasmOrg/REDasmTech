#pragma once

#include "abstractbuffer.h"
#include "memory.h"
#include <memory>
#include <redasm/segment.h>
#include <tl/optional.hpp>
#include <vector>

namespace redasm {

struct FileMapping {
    RDAddress base;
    usize length;
    RDOffset offset;
};

struct Program {
    ~Program();
    bool add_segment(std::string_view name, RDAddress start, RDAddress end,
                     u32 perm, u32 bits);
    bool map_file(RDOffset off, RDAddress start, RDAddress end);

    tl::optional<RDOffset> to_offset(RDAddress address) const;
    tl::optional<RDAddress> to_address(RDOffset offset) const;
    RDSegment* find_segment(RDAddress address);

    const RDSegment* find_segment(RDAddress address) const {
        return const_cast<Program*>(this)->find_segment(address);
    }

    std::vector<RDSegment> segments;
    std::vector<FileMapping> mappings;
    RDBuffer file;

    // OLD
    std::shared_ptr<AbstractBuffer> file_old;
    std::unique_ptr<Memory> memory_old;
};

} // namespace redasm
