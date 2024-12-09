#pragma once

#include <redasm/redasm.h>

namespace redasm {

struct Processor;

namespace builtins {

namespace processor {

void render_segment(const RDRendererParams* rp);
void render_function(const RDRendererParams* rp);

} // namespace processor

void register_processors();

} // namespace builtins
} // namespace redasm
