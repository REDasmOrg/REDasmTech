#pragma once

#include <redasm/redasm.h>

namespace redasm::builtins {

namespace processor {

void render_segment(const RDProcessor*, RDRenderer* r, const RDSegmentNew* seg);
void render_function(const RDProcessor*, RDRenderer* r, const RDFunction* func);

} // namespace processor

void register_processors();

} // namespace redasm::builtins
