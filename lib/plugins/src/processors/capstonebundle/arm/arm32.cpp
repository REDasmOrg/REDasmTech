#include "arm32.h"

ARM32Processor::ARM32Processor(cs_mode mode): Capstone{CS_ARCH_ARM, mode} {}

bool ARM32Processor::decode(RDInstruction* instr) {
    if(!this->disasm(instr, m_buffer.data(), m_buffer.size())) return false;

    const cs_arm& arm = this->insn->detail->arm;
    instr->id = this->insn->id;
    instr->length = this->insn->size;

    switch(this->insn->id) {
        case ARM_INS_B: {
            if(arm.cc != ARM_CC_AL)
                instr->features |= IF_JUMP;
            else
                instr->features |= IF_JUMP | IF_STOP;

            instr->operands[0].type = OP_ADDR;
            instr->operands[0].addr = arm.operands[0].imm & ~1;
            return true;
        }

        case ARM_INS_BL: {
            instr->features |= IF_CALL;
            instr->operands[0].type = OP_ADDR;
            instr->operands[0].addr = arm.operands[0].imm & ~1;
            return true;
        }

        case ARM_INS_POP:
        case ARM_INS_LDR:
        case ARM_INS_MOV: {
            if(this->is_pc(0)) instr->features |= IF_STOP;
            break;
        }

        default: break;
    }

    this->process_operands(instr);
    return true;
}

void ARM32Processor::emulate(RDEmulator* e, const RDInstruction* instr) const {
    if(instr->features & IF_JUMP)
        rdemulator_addref(e, instr->operands[0].addr, CR_JUMP);
    else if(instr->features & IF_CALL)
        rdemulator_addref(e, instr->operands[0].addr, CR_CALL);

    if(!(instr->features & IF_STOP))
        rdemulator_flow(e, instr->address + instr->length);
}

void ARM32Processor::render_instruction(RDRenderer* r,
                                        const RDInstruction* instr) const {
    const char* mnem = cs_insn_name(this->handle, instr->id);
    if(mnem)
        rdrenderer_mnem(r, mnem, THEME_DEFAULT);
    else
        rdrenderer_unkn(r);

    foreach_operand(i, op, instr) {
        if(i > 0) rdrenderer_text(r, ",");

        switch(op->type) {
            case OP_REG: rdrenderer_reg(r, op->reg); break;
            case OP_IMM: rdrenderer_cnst(r, op->imm); break;
            case OP_ADDR: rdrenderer_addr(r, op->addr); break;
            default: rdrenderer_unkn(r); break;
        }
    }
}

void ARM32Processor::process_operands(RDInstruction* instr) const {
    const cs_arm& arm = this->insn->detail->arm;

    for(u8 i = 0; i < std::min<u8>(RD_NOPERANDS, arm.op_count); i++) {
        const cs_arm_op& op = arm.operands[i];

        switch(op.type) {
            case ARM_OP_REG: {
                instr->operands[i].type = OP_REG;
                instr->operands[i].reg = op.reg;
                break;
            }

            case ARM_OP_IMM: {
                instr->operands[i].type = OP_IMM;
                instr->operands[i].imm = op.imm;
                break;
            }

            default: break;
        }
    }
}

bool ARM32Processor::is_pc(int opidx) const {
    if(opidx >= this->insn->detail->arm.op_count) return false;

    return this->insn->detail->arm.operands[opidx].type == ARM_OP_REG &&
           this->insn->detail->arm.operands[opidx].reg == ARM_REG_PC;
}
