#pragma once

#include <redasm/redasm.h>

namespace arm32 {

struct ARMCommon;

void check_macro(const ARMCommon* arm, RDInstruction* instr);

} // namespace arm32
