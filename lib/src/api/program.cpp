#include "../context.h"
#include "../state.h"
#include <redasm/program.h>
#include <spdlog/spdlog.h>

bool rd_addsegment(const char* name, RDAddress start, RDAddress end, u32 perm,
                   u32 bits) {
    spdlog::trace("rd_addsegment('{}', {:x}, {:x}, {:x}, {})", name, start, end,
                  perm, bits);

    return name && redasm::state::context &&
           redasm::state::context->program.add_segment(name, start, end, perm,
                                                       bits);
}

bool rd_addsegment_n(const char* name, RDAddress base, usize n, u32 perm,
                     u32 bits) {
    return rd_addsegment(name, base, base + n, perm, bits);
}

usize rd_getsegments(const RDSegment** segments) {
    spdlog::trace("rd_getsegments({})", fmt::ptr(segments));
    if(!redasm::state::context) return 0;
    if(segments) *segments = redasm::state::context->program.segments.data();
    return redasm::state::context->program.segments.size();
}

const Map(RDProgramRegion) rd_getregions() {
    spdlog::trace("rd_getregions()");
    if(!redasm::state::context) return nullptr;
    return redasm::state::context->program.regions;
}

bool rd_mapfile(RDOffset off, RDAddress start, RDAddress end) {
    spdlog::trace("rd_mapfile({:x}, {:x}, {:x})", off, start, end);

    return redasm::state::context &&
           redasm::state::context->program.map_file(off, start, end);
}

bool rd_mapfile_n(RDOffset off, RDAddress base, usize n) {
    return rd_mapfile(off, base, base + n);
}
