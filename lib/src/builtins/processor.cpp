#include "processor.h"
#include "../api/internal/byte.h"
#include "../api/internal/processor.h"
#include "../api/internal/redasm.h"
#include "../api/internal/renderer.h"
#include "../error.h"

namespace redasm::builtins {

namespace processor {

bool render_segment(const RDRendererParams* rp) {
    assume(rp);

    const RDSegment* segments = nullptr;
    usize c = api::internal::get_segments(&segments);
    assume(rp->segment_index < c);

    RDSegment seg;
    assume(api::internal::address_to_segment(rp->address, &seg));

    std::string start = api::internal::to_hex(seg.address);
    std::string end = api::internal::to_hex(seg.endaddress);

    std::string s =
        fmt::format("segment {} (start: {}, end: {})", seg.name, start, end);

    api::internal::renderer_themed(rp->renderer, s, THEME_SEGMENT);
    return true;
}

bool render_function(const RDRendererParams* rp) {
    std::string n = api::internal::get_name(rp->address);
    if(n.empty())
        n = "???";

    std::string s;

    if(api::internal::byte_has(rp->byte, BF_EXPORT))
        s = fmt::format("export function {}()", n);
    else
        s = fmt::format("function {}()", n);

    api::internal::renderer_themed(rp->renderer, s, THEME_FUNCTION);
    return true;
}

} // namespace processor

void register_processors() {
    RDProcessor nullprocessor{
        .id = "null",
        .name = "Null",
    };

    nullprocessor.rendersegment = [](const RDProcessor*,
                                     const RDRendererParams* rp) {
        return builtins::processor::render_segment(rp);
    };

    nullprocessor.renderfunction = [](const RDProcessor*,
                                      const RDRendererParams* rp) {
        return builtins::processor::render_function(rp);
    };

    api::internal::register_processor(nullprocessor);
}

} // namespace redasm::builtins
