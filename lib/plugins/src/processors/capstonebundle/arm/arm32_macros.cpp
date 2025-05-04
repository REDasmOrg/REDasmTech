#include "arm32_macros.h"
#include "arm32.h"
#include <capstone/capstone.h>

namespace arm32 {

void check_macro(const ARMCommon* arm, RDInstruction* instr) {
    // ADD reg,PC,... -> ADR reg,LABEL
    if(instr->id == ARM_INS_ADD && instr->operands[1].reg == ARM_REG_PC) {
        instr->id = ARM_INS_ADR;
        rdinstruction_setmnemonic(instr, "adr");

        instr->operands[1].type = OP_IMM;
        instr->operands[1].imm = arm->pc(instr) + instr->operands[2].imm;
        instr->operands[2].type = OP_NULL; // Delete operand 3
    }
}

} // namespace arm32
