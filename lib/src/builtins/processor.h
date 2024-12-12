#pragma once

#include <redasm/redasm.h>

namespace redasm {

struct Processor;

namespace builtins {

namespace processor {

void render_segment(const RDProcessor*, RDRenderer* r,
                    const RDSegment* segment);
void render_function(const RDProcessor*, RDRenderer* r,
                     const RDFunction* function);

} // namespace processor

void register_processors();

} // namespace builtins
} // namespace redasm
