#include "mips_decoder.h"
#include <climits>
#include <optional>
#include <redasm/redasm.h>
#include <string>

namespace {

std::optional<RDAddress> calc_addr(const MIPSDecodedInstruction& dec,
                                   RDAddress address) {
    if(dec.opcode->category == MIPS_CATEGORY_MACRO) {
        switch(dec.opcode->id) {
            case MIPS_MACRO_B: {
                return address + sizeof(MIPSInstruction) +
                       static_cast<i32>(mips_decoder::sign_ext(
                           dec.instr.i_s.immediate << 2, 32));
            }

            default: {
                // rd_addproblem(address, ("Cannot calculate address of '" +
                //                         std::string{dec.opcode->mnemonic} +
                //                         "'")
                //                            .c_str());
                break;
            }
        }
    }
    else {
        switch(dec.opcode->encoding) {
            case MIPS_ENCODING_J: {
                return (address & (0xF << ((sizeof(u32) * CHAR_BIT) - 4))) |
                       (static_cast<u32>(dec.instr.j.target) << 2);
            }

            case MIPS_ENCODING_I: {
                return address + sizeof(MIPSInstruction) +
                       static_cast<i32>(mips_decoder::sign_ext(
                           dec.instr.i_s.immediate << 2, 32));
            }

            default: {
                // rd_addproblem(address, ("Cannot calculate address of '" +
                //                         std::string{dec.opcode->mnemonic} +
                //                         "'")
                //                            .c_str());
                break;
            }
        }
    }

    return std::nullopt;
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

    switch(instr->uservalue) {
        case MIPS_CATEGORY_JUMP: theme = THEME_JUMP; break;
        case MIPS_CATEGORY_JUMPCOND: theme = THEME_JUMPCOND; break;
        case MIPS_CATEGORY_CALL: theme = THEME_CALL; break;
        case MIPS_CATEGORY_RET: theme = THEME_RET; break;
        default: break;
    }

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
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = dec.instr.i_u.rt;
            instr->operands[1].type = OP_REG;
            instr->operands[1].reg = dec.instr.i_u.rs;
            break;
        }

        case MIPS_MACRO_LI: {
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = dec.instr.i_u.rt;
            instr->operands[1].type = OP_IMM;
            instr->operands[1].reg = dec.instr.i_u.immediate;
            break;
        }

        case MIPS_MACRO_LA: {
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = dec.macro.regimm.reg;
            instr->operands[1].type = OP_IMM;
            instr->operands[1].reg = dec.macro.regimm.address;
            instr->operands[1].dtype.id = TID_U32;
            break;
        }

        case MIPS_MACRO_LW:
        case MIPS_MACRO_SW: {
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = dec.macro.regimm.reg;
            instr->operands[1].type = OP_MEM;
            instr->operands[1].reg = dec.macro.regimm.address;
            instr->operands[1].dtype.id = TID_U32;
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

template<bool BigEndian>
void decode(const RDProcessor*, RDInstruction* instr) {
    MIPSDecodedInstruction dec;
    if(!mips_decoder::decode(instr->address, dec, BigEndian))
        return;

    instr->id = dec.opcode->id;
    instr->length = dec.length;
    instr->uservalue = dec.opcode->category;

    switch(dec.opcode->category) {
        case MIPS_CATEGORY_CALL: instr->features |= INSTR_CALL; break;

        case MIPS_CATEGORY_JUMP:
            instr->features |= INSTR_JUMP | INSTR_STOP;
            break;

        case MIPS_CATEGORY_JUMPCOND: instr->features |= INSTR_JUMP; break;
        case MIPS_CATEGORY_RET: instr->features |= INSTR_STOP; break;
        default: break;
    }

    switch(dec.opcode->id) {
        case MIPS_INSTR_LB:
        case MIPS_INSTR_LBU:
        case MIPS_INSTR_LW:
        case MIPS_INSTR_LWL:
        case MIPS_INSTR_LWR:
        case MIPS_INSTR_SB:
        case MIPS_INSTR_SH:
        case MIPS_INSTR_SW: {
            instr->operands[0].type = OP_REG;
            instr->operands[1].type = OP_DISPL;
            instr->operands[1].displ.displ = dec.instr.i_u.immediate;
            instr->operands[1].displ.base = dec.instr.i_u.rs;
            return;
        }

        default: break;
    }

    if(dec.opcode->category == MIPS_CATEGORY_MACRO) {
        decode_macro(dec, instr);
        return;
    }
}

void emulate(const RDProcessor*, RDEmulator* e, const RDInstruction* instr) {
    foreach_operand(i, op, instr) {
        switch(op->type) {
            case OP_IMM: {
                if(instr->features & INSTR_JUMP)
                    rdemulator_addref(e, instr->operands[0].imm, CR_JUMP);
                else if(instr->features & INSTR_CALL)
                    rdemulator_addref(e, instr->operands[0].imm, CR_CALL);
                else if(op->userdata1)
                    rdemulator_addref(e, op->imm, DR_ADDRESS);
                break;
            };

            default: break;
        }
    }

    if(!(instr->features & INSTR_STOP))
        rdemulator_addref(e, instr->address + instr->length, CR_FLOW);
}

void render_instruction(const RDProcessor*, RDRenderer* r,
                        const RDInstruction* instr) {

    render_mnemonic(instr, r);

    foreach_operand(i, op, instr) {
        if(i > 0)
            rdrenderer_text(r, ", ");

        switch(op->type) {
            case OP_REG: {
                rdrenderer_reg(r, mips_decoder::reg(op->reg));
                break;
            }

            case OP_IMM: {
                if(instr->id == MIPS_MACRO_LA)
                    rdrenderer_addr(r, op->imm);
                else
                    rdrenderer_cnst(r, op->imm);
                break;
            };

            case OP_MEM: {
                rdrenderer_addr(r, op->mem);
                break;
            }

            case OP_DISPL: {
                rdrenderer_cnst(r, op->displ.displ);
                rdrenderer_text(r, "(");
                rdrenderer_reg(r, mips_decoder::reg(op->displ.base));
                rdrenderer_text(r, ")");
                break;
            }

            default: break;
        }
    }
}

} // namespace

void rdplugin_init() {
    mips_initialize_formats();

    RDProcessor mips32le{};
    mips32le.id = "mips32le";
    mips32le.name = "MIPS32 (Little Endian)";
    mips32le.decode = decode<false>;
    mips32le.emulate = emulate;
    mips32le.renderinstruction = render_instruction;
    rd_registerprocessor(&mips32le);

    RDProcessor mips32be{};
    mips32be.id = "mips32be";
    mips32be.name = "MIPS32 (Big Endian)";
    mips32be.decode = decode<true>;
    mips32be.emulate = emulate;
    mips32be.renderinstruction = render_instruction;
    rd_registerprocessor(&mips32be);

    // RDProcessor mips64le{};
    // ...
    // rd_registerprocessor(&mips64le);

    // RDProcessor mips64be{};
    // ...
    // rd_registerprocessor(&mips64be);
}
