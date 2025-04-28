#include "arm32.h"

namespace arm32 {

namespace {

const int SEGMENT_REGISTERS[] = {ARM_REG_T, -1};

} // namespace

ARMCommon::ARMCommon(cs_mode mode): Capstone{CS_ARCH_ARM, mode} {}
const int* ARMCommon::get_segmentregisters() const { return SEGMENT_REGISTERS; }

const char* ARMCommon::get_registername(int reg) const {
    if(reg == ARM_REG_T) return "T";
    return Capstone::get_registername(reg);
}

RDAddress ARMCommon::normalize_address(RDAddress address) const {
    RDAddress naddr = address & ~1;
    rd_setsreg(naddr, ARM_REG_T, address & 1);
    return naddr;
}

bool ARMCommon::decode(RDInstruction* instr) {
    if(!this->disasm(instr, m_buffer.data(), m_buffer.size())) return false;

    const cs_arm& arm = this->insn->detail->arm;
    instr->id = this->insn->id;
    instr->length = this->insn->size;
    if(arm.writeback) instr->uservalue1 = true;

    switch(this->insn->id) {
        case ARM_INS_B: {
            if(arm.cc != ARM_CC_AL)
                instr->features |= IF_JUMP;
            else
                instr->features |= IF_JUMP | IF_STOP;

            instr->operands[0].type = OP_ADDR;
            instr->operands[0].addr = arm.operands[0].imm;
            return true;
        }

        case ARM_INS_BLX:
        case ARM_INS_BL: {
            instr->features |= IF_CALL;
            instr->operands[0].type = OP_ADDR;
            instr->operands[0].addr = arm.operands[0].imm;
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

void ARMCommon::emulate(RDEmulator* e, const RDInstruction* instr) const {
    if(instr->features & IF_JUMP) {
        rdemulator_addref(e, instr->operands[0].addr, CR_JUMP);
    }
    else if(instr->features & IF_CALL) {
        if(instr->id == ARM_INS_BLX) {
            rdemulator_addref(
                e, this->normalize_address(instr->operands[0].addr), CR_CALL);
        }
        else
            rdemulator_addref(e, instr->operands[0].addr, CR_CALL);
    }

    if(!(instr->features & IF_STOP))
        rdemulator_flow(e, instr->address + instr->length);
}

void ARMCommon::render_instruction(RDRenderer* r,
                                   const RDInstruction* instr) const {
    rdrenderer_mnem(r, instr, THEME_DEFAULT);

    foreach_operand(i, op, instr) {
        if(i > 0) rdrenderer_text(r, ",");

        switch(op->type) {
            case OP_REG: rdrenderer_reg(r, op->reg); break;
            case OP_IMM: rdrenderer_cnst(r, op->imm); break;
            case OP_ADDR: rdrenderer_addr(r, op->addr); break;

            case OP_MEM: {
                rdrenderer_text(r, "[");
                rdrenderer_addr(r, op->mem);
                rdrenderer_text(r, "]");
                if(instr->uservalue1) rdrenderer_text(r, "!");
                break;
            }

            case OP_PHRASE: {
                rdrenderer_text(r, "[");
                rdrenderer_reg(r, op->phrase.base);
                rdrenderer_text(r, "+");
                rdrenderer_reg(r, op->phrase.index);
                rdrenderer_text(r, "]");
                if(instr->uservalue1) rdrenderer_text(r, "!");
                break;
            }

            case OP_DISPL: {
                rdrenderer_text(r, "[");
                rdrenderer_reg(r, op->displ.base);
                rdrenderer_text(r, "+");
                rdrenderer_reg(r, op->displ.index);
                rdrenderer_cnst_ex(r, op->displ.s_displ, 0, RC_NEEDSIGN);
                rdrenderer_text(r, "]");
                if(instr->uservalue1) rdrenderer_text(r, "!");
                break;
            }

            default: rdrenderer_unkn(r); break;
        }
    }
}

void ARMCommon::process_operands(RDInstruction* instr) const {
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

            case ARM_OP_MEM: {
                if(op.mem.base == ARM_REG_INVALID &&
                   op.mem.index == ARM_REG_INVALID) {
                    instr->operands[i].type = OP_MEM;
                    instr->operands[i].mem = op.mem.disp;
                }
                else if(!op.mem.disp) {
                    instr->operands[i].type = OP_PHRASE;
                    instr->operands[i].displ.base = op.mem.base;
                    instr->operands[i].displ.index = op.mem.index;
                }
                else {
                    instr->operands[i].type = OP_DISPL;
                    instr->operands[i].displ.base = op.mem.base;
                    instr->operands[i].displ.index = op.mem.index;
                    instr->operands[i].displ.s_displ = op.mem.disp;
                }

                break;
            }

            default: break;
        }
    }
}

bool ARMCommon::is_pc(int opidx) const {
    if(opidx >= this->insn->detail->arm.op_count) return false;

    return this->insn->detail->arm.operands[opidx].type == ARM_OP_REG &&
           this->insn->detail->arm.operands[opidx].reg == ARM_REG_PC;
}

} // namespace arm32
