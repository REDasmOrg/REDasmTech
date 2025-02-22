#pragma once

#include "../disasm/function.h"
#include <redasm/buffer.h>
#include <redasm/program.h>
#include <redasm/segment.h>
#include <redasm/types.h>
#include <string_view>
#include <tl/optional.hpp>
#include <vector>

namespace redasm {

struct FileMapping {
    RDAddress base;
    usize length;
    RDOffset offset;
};

struct Program {
    Program();
    ~Program();
    bool map_file(RDOffset off, RDAddress start, RDAddress end);

    bool add_segment(std::string_view name, RDAddress start, RDAddress end,
                     u32 perm, u32 bits);

    tl::optional<RDOffset> to_offset(RDAddress address) const;
    tl::optional<RDAddress> to_address(RDOffset offset) const;
    RDSegment* find_segment(RDAddress address);
    Function* find_function(RDAddress address);

    const RDSegment* find_segment(RDAddress address) const {
        return const_cast<Program*>(this)->find_segment(address);
    }

    const Function* find_function(RDAddress address) const {
        return const_cast<Program*>(this)->find_function(address);
    }

    std::vector<FileMapping> mappings;
    std::vector<RDSegment> segments;
    std::vector<Function> functions;
    Map(RDProgramRegion) regions;
    RDBuffer* file;
};

} // namespace redasm
