#pragma once

#include <Zydis/Zydis.h>
#include <redasm/redasm.h>

namespace x86_lifter {

bool lift(RDProcessor*, RDILList* l, const RDInstruction* instr);

}
