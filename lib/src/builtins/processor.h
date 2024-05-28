#pragma once

#include <redasm/redasm.h>

namespace redasm {

struct Processor;

namespace builtins {

namespace processor {

bool render_segment(const RDRendererParams* rp);
bool render_function(const RDRendererParams* rp);

} // namespace processor

void register_processors();

} // namespace builtins
} // namespace redasm
