#include "arm32.h"
#include "arm32_macros.h"

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

RDAddress ARMCommon::normalize_address(RDAddress address, bool query) const {
    RDAddress naddr = address & ~1;
    if(!query) rd_setsreg(naddr, ARM_REG_T, address & 1);
    return naddr;
}

RDAddress ARMCommon::pc(const RDInstruction* instr) const {
    /*
     * https://community.arm.com/support-forums/f/architectures-and-processors-forum/4030/i-need-an-explication-to-the-armv6-manual
     * https://stackoverflow.com/questions/24091566/why-does-the-arm-pc-register-point-to-the-instruction-after-the-next-one-to-be-e
     * https://stackoverflow.com/questions/2102921/strange-behaviour-of-ldr-pc-value
     *
     * ARM state:
     *  - The value of the PC is the address of the current instruction plus 8
     * bytes.
     *  - Bits [1:0] of this value are always zero, because ARM instructions are
     * always word-aligned.
     *
     * THUMB state:
     *  - The value read is the address of the instruction plus 4 bytes.
     *  - Bit [0] of this value is always zero, because Thumb instructions are
     * always halfword-aligned.
     */

    if(this->mode & CS_MODE_THUMB) return instr->address + (sizeof(u16) * 2);
    return instr->address + (sizeof(u32) * 2);
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

        case ARM_INS_ADR: {
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = arm.operands[0].reg;
            instr->operands[1].type = OP_ADDR;
            instr->operands[1].addr = this->pc(instr) + arm.operands[1].imm;
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

    if(!this->check_fixups(instr, arm)) this->process_operands(instr);
    arm32::check_macro(this, instr);
    return true;
}

void ARMCommon::emulate(RDEmulator* e, const RDInstruction* instr) const {
    if(instr->features & IF_JUMP) {
        rdemulator_addref(e, instr->operands[0].addr, CR_JUMP);
    }
    else if(instr->features & IF_CALL) {
        if(instr->id == ARM_INS_BLX) {
            rdemulator_addref(e, instr->operands[0].addr, CR_CALL);
        }
        else
            rdemulator_addref(e, instr->operands[0].addr, CR_CALL);
    }
    else {
        foreach_operand(i, op, instr) {
            if(op->type == OP_ADDR)
                rdemulator_addref(e, op->mem, DR_ADDRESS);
            else if(op->type == OP_MEM)
                rdemulator_addref(e, op->mem, DR_READ);
        }
    }

    if(!(instr->features & IF_STOP))
        rdemulator_flow(e, instr->address + instr->length);
}

void ARMCommon::render_instruction(RDRenderer* r,
                                   const RDInstruction* instr) const {
    if(instr->features & IF_CALL)
        rdrenderer_mnem(r, instr, THEME_CALL);
    else if(instr->features & IF_JUMP) {
        bool uncond = instr->features & IF_STOP;
        rdrenderer_mnem(r, instr, uncond ? THEME_JUMP : THEME_JUMPCOND);
    }
    else if(instr->features & IF_STOP)
        rdrenderer_mnem(r, instr, THEME_RET);
    else
        rdrenderer_mnem(r, instr, THEME_DEFAULT);

    if(instr->id == ARM_INS_PUSH || instr->id == ARM_INS_POP)
        rdrenderer_text(r, "{");

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
                rdrenderer_text(r, ",");
                rdrenderer_reg(r, op->phrase.index);
                rdrenderer_text(r, "]");
                if(instr->uservalue1) rdrenderer_text(r, "!");
                break;
            }

            case OP_DISPL: {
                rdrenderer_text(r, "[");
                rdrenderer_reg(r, op->displ.base);

                if(op->displ.index != ARM_REG_INVALID) {
                    rdrenderer_text(r, ",");
                    rdrenderer_reg(r, op->displ.index);
                }

                if(op->displ.s_displ) {
                    rdrenderer_text(r, ",");
                    rdrenderer_text(r, "#");
                    rdrenderer_cnst(r, op->displ.s_displ);
                }

                rdrenderer_text(r, "]");
                if(instr->uservalue1) rdrenderer_text(r, "!");
                break;
            }

            default: rdrenderer_unkn(r); break;
        }
    }

    if(instr->id == ARM_INS_PUSH || instr->id == ARM_INS_POP)
        rdrenderer_text(r, "}");
}

void ARMCommon::process_operand(RDInstruction* instr, usize idx,
                                const cs_arm_op& op) const {
    switch(op.type) {
        case ARM_OP_REG: {
            instr->operands[idx].type = OP_REG;
            instr->operands[idx].reg = op.reg;
            break;
        }

        case ARM_OP_IMM: {
            instr->operands[idx].type = OP_IMM;
            instr->operands[idx].imm = op.imm;
            break;
        }

        case ARM_OP_MEM: {
            if(op.mem.base == ARM_REG_INVALID &&
               op.mem.index == ARM_REG_INVALID) {
                instr->operands[idx].type = OP_ADDR;
                instr->operands[idx].mem = op.mem.disp;
            }
            else if(op.mem.base == ARM_REG_PC) {
                instr->operands[idx].type = OP_ADDR;
                instr->operands[idx].mem = this->pc(instr) + op.mem.disp;
            }
            else {
                instr->operands[idx].type = OP_DISPL;
                instr->operands[idx].displ.base = op.mem.base;
                instr->operands[idx].displ.index = op.mem.index;
                instr->operands[idx].displ.s_displ = op.mem.disp;
            }

            break;
        }

        default: break;
    }
}

bool ARMCommon::check_fixups(RDInstruction* instr, const cs_arm& arm) const {
    // https://github.com/capstone-engine/capstone/issues/2373
    if(arm.op_count == 4) {
        if(this->insn->id == ARM_INS_ADD) {
            this->process_operand(instr, 0, arm.operands[0]);
            this->process_operand(instr, 1, arm.operands[1]);

            u32 imm = rd_ror32(arm.operands[2].imm, arm.operands[3].imm);
            instr->operands[2].type = OP_IMM;
            instr->operands[2].imm = imm;
            return true;
        }

        rd_addproblem(instr->address, "Istruzione Brutta");
    }

    return false;
}

void ARMCommon::process_operands(RDInstruction* instr) const {
    const cs_arm& arm = this->insn->detail->arm;
    for(u8 i = 0; i < std::min<u8>(RD_NOPERANDS, arm.op_count); i++)
        this->process_operand(instr, i, arm.operands[i]);
}

bool ARMCommon::is_pc(int opidx) const {
    if(opidx >= this->insn->detail->arm.op_count) return false;

    return this->insn->detail->arm.operands[opidx].type == ARM_OP_REG &&
           this->insn->detail->arm.operands[opidx].reg == ARM_REG_PC;
}

} // namespace arm32
