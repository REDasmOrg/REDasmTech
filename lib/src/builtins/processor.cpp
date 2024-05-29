#include "processor.h"
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

    const RDSegment& seg = segments[rp->segment_index];
    std::string start = api::internal::to_hex(seg.address);
    std::string end = api::internal::to_hex(seg.endaddress);

    std::string s =
        fmt::format("SEGMENT {} (START: {}, END: {})", seg.name, start, end);

    api::internal::renderer_themed(rp->renderer, s, THEME_SEGMENT);
    return true;
}

bool render_function(const RDRendererParams* rp) {
    std::string n = api::internal::get_name(rp->address);
    if(n.empty())
        n = "???";

    std::string s = fmt::format("FUNCTION {}", n);
    api::internal::renderer_themed(rp->renderer, s, THEME_FUNCTION);
    return true;
}

} // namespace processor

void register_processors() {
    RDProcessor nullprocessor{};
    nullprocessor.name = "";
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