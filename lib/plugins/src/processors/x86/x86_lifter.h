#pragma once

#include "x86_common.h"
#include <Zydis/Zydis.h>
#include <redasm/redasm.h>

namespace x86_lifter {

bool lift(const X86Instruction& instr, RDILList* l);

}
