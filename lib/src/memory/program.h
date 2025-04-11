#pragma once

#include "../disasm/function.h"
#include <redasm/buffer.h>
#include <redasm/program.h>
#include <redasm/segment.h>
#include <redasm/sreg.h>
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

    bool add_sreg_range(RDAddress start, RDAddress end, int sreg, u64 v);
    bool set_sreg(RDAddress address, int sreg, const RDRegValue& val);
    tl::optional<RDOffset> to_offset(RDAddress address) const;
    tl::optional<RDAddress> to_address(RDOffset offset) const;
    RDSRange* find_sreg_range(RDAddress address, int sreg);
    RDSegment* find_segment(RDAddress address);
    Function* find_function(RDAddress address);

    const RDSegment* find_segment(RDAddress address) const {
        return const_cast<Program*>(this)->find_segment(address);
    }

    const Function* find_function(RDAddress address) const {
        return const_cast<Program*>(this)->find_function(address);
    }

    std::vector<FileMapping> mappings;
    RDSegmentSlice segments;
    std::vector<Function> functions;
    RDSRangeMap segmentregs;
    RDBuffer* file;
};

} // namespace redasm
