#include "mips_decoder.h"
#include "mips_registers.h"
#include <climits>
#include <redasm/redasm.h>

namespace {

RDAddress calc_addr16(RDAddress pc, u16 imm) {
    i32 tgt = mips_decoder::signext_16_32(imm);
    return pc + sizeof(MIPSInstruction) + (static_cast<i32>(tgt * 4));
}

RDAddress calc_addr26(RDAddress pc, u32 imm) {
    u32 pcupper = (pc + sizeof(MIPSInstruction)) & 0xF0000000;
    return pcupper | imm << 2;
}

bool is_jump_cond(const RDInstruction* instr) {
    switch(instr->id) {
        case MIPS_INSTR_BEQ:
        case MIPS_INSTR_BNE:
        case MIPS_INSTR_BGEZ:
        case MIPS_INSTR_BGTZ:
        case MIPS_MACRO_BEQZ:
        case MIPS_MACRO_BNEZ: return true;

        default: break;
    }

    return false;
}

void render_mnemonic(const RDInstruction* instr, RDRenderer* r) {
    switch(instr->id) {
        case MIPS_MACRO_NOP:
            rdrenderer_mnem(r, mips_decoder::mnemonic(instr->id), THEME_NOP);
            return;

        case MIPS_MACRO_B:
            rdrenderer_mnem(r, mips_decoder::mnemonic(instr->id), THEME_JUMP);
            return;

        default: break;
    }

    RDThemeKind theme = THEME_DEFAULT;

    if(instr->features & IF_JUMP)
        theme = is_jump_cond(instr) ? THEME_JUMPCOND : THEME_JUMP;
    if(instr->features & IF_CALL)
        theme = THEME_CALL;
    if(instr->features & IF_STOP)
        theme = THEME_RET;

    rdrenderer_mnem(r, mips_decoder::mnemonic(instr->id), theme);
}

void decode_macro(const MIPSDecodedInstruction& dec, RDInstruction* instr) {
    switch(dec.opcode->id) {
        case MIPS_MACRO_MOVE: {
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = dec.instr.r.rd;
            instr->operands[1].type = OP_REG;
            instr->operands[1].reg = dec.instr.r.rs;
            break;
        }

        case MIPS_MACRO_B: {
            instr->features = IF_JUMP | IF_STOP;
            instr->operands[0].type = OP_ADDR;
            instr->operands[0].addr =
                calc_addr16(instr->address, dec.instr.i_s.imm);
            break;
        }

        case MIPS_MACRO_BEQZ:
        case MIPS_MACRO_BNEZ: {
            instr->features = IF_JUMP;

            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = dec.macro.regimm.reg;
            instr->operands[1].type = OP_ADDR;
            instr->operands[1].addr =
                calc_addr16(instr->address, dec.macro.regimm.s_imm16);
            break;
        }

        case MIPS_MACRO_LI: {
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = dec.instr.i_u.rt;
            instr->operands[1].type = OP_IMM;
            instr->operands[1].reg = dec.instr.i_u.imm;
            break;
        }

        case MIPS_MACRO_LA: {
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = dec.macro.regimm.reg;
            instr->operands[1].type = OP_ADDR;
            instr->operands[1].addr = dec.macro.regimm.address;
            break;
        }

        case MIPS_MACRO_LW:
        case MIPS_MACRO_SW: {
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = dec.macro.regimm.reg;
            instr->operands[1].type = OP_MEM;
            instr->operands[1].mem = dec.macro.regimm.address;
            break;
        }

        case MIPS_MACRO_LHU:
        case MIPS_MACRO_SH: {
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = dec.macro.regimm.reg;
            instr->operands[1].type = OP_MEM;
            instr->operands[1].reg = dec.macro.regimm.address;
            break;
        }

        case MIPS_MACRO_NOP: break;

        default: {
            // rd_addproblem(instr->address,
            //               ("Unhandled instruction: '" +
            //                std::string{dec.opcode->mnemonic} + "'")
            //                   .c_str());
            break;
        }
    }
}

void decode_r(const MIPSDecodedInstruction& dec, RDInstruction* instr) {
    switch(instr->id) {
        case MIPS_INSTR_JALR:
        case MIPS_INSTR_JR:
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = dec.instr.r.rs;
            break;

        default: {
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = dec.instr.r.rd;
            instr->operands[1].type = OP_REG;
            instr->operands[1].reg = dec.instr.r.rs;
            instr->operands[2].type = OP_REG;
            instr->operands[2].reg = dec.instr.r.rt;
            break;
        }
    }
}

void decode_i(const MIPSDecodedInstruction& dec, RDInstruction* instr) {
    switch(dec.opcode->id) {
        case MIPS_INSTR_LB:
        case MIPS_INSTR_LBU:
        case MIPS_INSTR_LH:
        case MIPS_INSTR_LHU:
        case MIPS_INSTR_LW:
        case MIPS_INSTR_LWL:
        case MIPS_INSTR_LWR:
        case MIPS_INSTR_SB:
        case MIPS_INSTR_SH:
        case MIPS_INSTR_SWL:
        case MIPS_INSTR_SWR:
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = dec.instr.i_u.rt;

            instr->operands[1].type = OP_DISPL;
            instr->operands[1].displ.base = dec.instr.i_u.rs;
            instr->operands[1].displ.displ = dec.instr.i_u.imm;
            return;

        case MIPS_INSTR_LUI:
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = dec.instr.i_u.rt;
            instr->operands[1].type = OP_IMM;
            instr->operands[1].imm = dec.instr.i_u.imm;
            return;

        default: break;
    }

    instr->operands[0].type = OP_REG;
    instr->operands[0].reg = dec.instr.i_u.rt;

    instr->operands[1].type = OP_REG;
    instr->operands[1].reg = dec.instr.i_u.rs;

    if(dec.opcode->category == MIPS_CATEGORY_JUMPCOND) {
        instr->operands[2].type = OP_ADDR;
        instr->operands[2].addr =
            calc_addr16(instr->address, dec.instr.i_s.imm);
    }
    else {
        instr->operands[2].type = OP_IMM;
        instr->operands[2].imm = dec.instr.i_u.imm;
    }
}

void decode_j(const MIPSDecodedInstruction& dec, RDInstruction* instr) {
    instr->operands[0].type = OP_ADDR;
    instr->operands[0].addr =
        calc_addr26(instr->address, static_cast<u32>(dec.instr.j.target));
}

void decode_b(const MIPSDecodedInstruction& dec, RDInstruction* instr) {
    if(dec.opcode->id == MIPS_INSTR_BREAK)
        instr->features |= IF_STOP;
}

template<bool BigEndian>
void decode(const RDProcessor*, RDInstruction* instr) {
    MIPSDecodedInstruction dec;
    if(!mips_decoder::decode(instr->address, dec, BigEndian))
        return;

    instr->id = dec.opcode->id;
    instr->length = dec.length;
    instr->uservalue = dec.opcode->format;

    if(dec.opcode->category == MIPS_CATEGORY_MACRO) {
        decode_macro(dec, instr);
        return;
    }

    switch(dec.opcode->format) {
        case MIPS_FORMAT_R: decode_r(dec, instr); break;
        case MIPS_FORMAT_I: decode_i(dec, instr); break;
        case MIPS_FORMAT_J: decode_j(dec, instr); break;
        case MIPS_FORMAT_B: decode_b(dec, instr); break;
        default: break;
    }

    switch(dec.opcode->category) {
        case MIPS_CATEGORY_CALL: instr->features |= IF_CALL; break;
        case MIPS_CATEGORY_JUMP: instr->features |= IF_JUMP | IF_STOP; break;
        case MIPS_CATEGORY_JUMPCOND: instr->features |= IF_JUMP; break;
        case MIPS_CATEGORY_RET: instr->features |= IF_STOP; break;
        default: break;
    }
}

void handle_operand(int index, const RDOperand* op) {}

void emulate(const RDProcessor*, RDEmulator* e, const RDInstruction* instr) {
    rdemulator_setreg(e, MIPS_REG_ZERO, 0); // Force register $zero

    switch(instr->id) {
        case MIPS_MACRO_LA:
            rdemulator_addref(e, instr->operands[1].addr, DR_ADDRESS);
            return;

        case MIPS_MACRO_LW:
            rdemulator_addref(e, instr->operands[1].addr, DR_READ);
            return;

        case MIPS_MACRO_SW:
        case MIPS_MACRO_SH:
            rdemulator_addref(e, instr->operands[1].addr, DR_WRITE);
            return;

        default: break;
    }

    foreach_operand(i, op, instr) {
        switch(op->type) {
            case OP_ADDR: {
                if(instr->features & IF_CALL)
                    rdemulator_addref(e, op->addr, CR_CALL);
                else if(instr->features & IF_JUMP)
                    rdemulator_addref(e, op->addr, CR_JUMP);
                else
                    handle_operand(i, op);
                break;
            };

            default: break;
        }
    }
}

void render_instruction(const RDProcessor*, RDRenderer* r,
                        const RDInstruction* instr) {

    render_mnemonic(instr, r);

    foreach_operand(i, op, instr) {
        if(i > 0)
            rdrenderer_text(r, ", ");

        switch(op->type) {
            case OP_REG: rdrenderer_reg(r, op->reg); break;
            case OP_ADDR: rdrenderer_addr(r, op->addr); break;
            case OP_IMM: rdrenderer_cnst(r, op->imm); break;
            case OP_MEM: rdrenderer_addr(r, op->mem); break;

            case OP_DISPL:
                rdrenderer_cnst(r, op->displ.displ);
                rdrenderer_text(r, "(");
                rdrenderer_reg(r, op->displ.base);
                rdrenderer_text(r, ")");
                break;

            default: break;
        }
    }
}

const char* get_register_name(const RDProcessor*, int reg) {
    return mips_decoder::reg(reg);
}

} // namespace

void rdplugin_init() {
    mips_initialize_formats();

    RDProcessor mips32le{};
    mips32le.id = "mips32le";
    mips32le.name = "MIPS32 (Little Endian)";
    mips32le.address_size = 4;
    mips32le.integer_size = 4;
    mips32le.decode = decode<false>;
    mips32le.emulate = emulate;
    mips32le.getregistername = get_register_name;
    mips32le.renderinstruction = render_instruction;
    rd_registerprocessor(&mips32le);

    RDProcessor mips32be{};
    mips32be.id = "mips32be";
    mips32be.name = "MIPS32 (Big Endian)";
    mips32be.address_size = 8;
    mips32be.integer_size = 4;
    mips32be.decode = decode<true>;
    mips32be.emulate = emulate;
    mips32be.getregistername = get_register_name;
    mips32be.renderinstruction = render_instruction;
    rd_registerprocessor(&mips32be);

    // RDProcessor mips64le{};
    // ...
    // rd_registerprocessor(&mips64le);

    // RDProcessor mips64be{};
    // ...
    // rd_registerprocessor(&mips64be);
}
