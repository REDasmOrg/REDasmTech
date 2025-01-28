#include "processor.h"
#include "../plugins/pluginmanager.h"
#include <fmt/core.h>
#include <redasm/function.h>
#include <redasm/processor.h>

namespace redasm::builtins {

namespace processor {

void render_segment(const RDProcessor*, RDRenderer* r,
                    const RDSegment* segment) {
    const char* start = rd_tohex(segment->startaddr);
    const char* end = rd_tohex(segment->endaddr);

    std::string s = fmt::format("segment {} (start: {}, end: {})",
                                segment->name, start, end);

    rdrenderer_themed(r, s.c_str(), THEME_SEGMENT);
}

void render_function(const RDProcessor*, RDRenderer* r,
                     const RDFunction* function) {

    RDAddress ep = rdfunction_getentry(function);
    const char* n = rd_getname(ep);
    if(!n) n = "???";

    std::string s;

    if(rdfunction_isexport(function))
        s = fmt::format("export function {}()", n);
    else
        s = fmt::format("function {}()", n);

    rdrenderer_themed(r, s.c_str(), THEME_FUNCTION);
}

} // namespace processor

namespace {

RDProcessorPlugin null_processor = {
    .level = REDASM_API_LEVEL,
    .id = "null",
    .name = "Null",
    .flags = PF_LAST,
    .address_size = sizeof(uptr),
    .integer_size = sizeof(int),
    .render_segment = builtins::processor::render_segment,
    .render_function = builtins::processor::render_function,
};

} // namespace

void register_processors() {
    pm::register_processor(&null_processor, pm::NATIVE);
}

} // namespace redasm::builtins
