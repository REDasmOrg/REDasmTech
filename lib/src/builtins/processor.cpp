#include "processor.h"
#include "../api/internal/function.h"
#include "../api/internal/processor.h"
#include "../api/internal/redasm.h"
#include "../api/internal/renderer.h"
#include "../api/internal/utils.h"

namespace redasm::builtins {

namespace processor {

void render_segment(const RDProcessor*, RDRenderer* r,
                    const RDSegment* segment) {
    std::string start = api::internal::to_hex(segment->address);
    std::string end = api::internal::to_hex(segment->endaddress);

    std::string s = fmt::format("segment {} (start: {}, end: {})",
                                segment->name, start, end);

    api::internal::renderer_themed(r, s, THEME_SEGMENT);
}

void render_function(const RDProcessor*, RDRenderer* r,
                     const RDFunction* function) {

    RDAddress ep = api::internal::function_getentry(function);
    std::string n = api::internal::get_name(ep);
    if(n.empty()) n = "???";

    std::string s;

    if(api::internal::function_isexport(function))
        s = fmt::format("export function {}()", n);
    else
        s = fmt::format("function {}()", n);

    api::internal::renderer_themed(r, s, THEME_FUNCTION);
}

} // namespace processor

namespace {

RDProcessorPlugin null_processor = {
    .id = "null",
    .name = "Null",
    .address_size = sizeof(uptr),
    .integer_size = sizeof(int),
    .rendersegment = builtins::processor::render_segment,
    .renderfunction = builtins::processor::render_function,
};

} // namespace

void register_processors() {
    api::internal::register_processor(&null_processor);
}

} // namespace redasm::builtins
