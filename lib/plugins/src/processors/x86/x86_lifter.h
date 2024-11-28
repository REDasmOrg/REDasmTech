#pragma once

#include <Zydis/Zydis.h>
#include <redasm/redasm.h>

namespace x86_lifter {

bool lift(const RDProcessor*, const RDInstruction* instr, RDILList* l);

}
