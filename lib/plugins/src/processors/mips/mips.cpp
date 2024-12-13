#include "mips_decoder.h"
#include <climits>
#include <optional>
#include <redasm/redasm.h>

namespace {

std::optional<RDAddress> calc_addr(const MIPSDecodedInstruction& dec,
                                   RDAddress address) {
    if(dec.opcode->category == MIPS_CATEGORY_MACRO) {
        switch(dec.opcode->id) {
            case MIPS_MACRO_B: {
                i32 tgt = mips_decoder::signext_16_32(dec.instr.i_s.imm);
                return address + sizeof(MIPSInstruction) +
                       (static_cast<i32>(tgt * 4));
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
        switch(dec.opcode->format) {
            case MIPS_FORMAT_J: {
                u32 pcupper = (address + sizeof(MIPSInstruction)) & 0xF0000000;
                return pcupper | static_cast<u32>(dec.instr.j.target) << 2;
            }

            case MIPS_FORMAT_I: {
                i32 tgt = mips_decoder::signext_16_32(dec.instr.i_s.imm);
                return address + sizeof(MIPSInstruction) +
                       (static_cast<i32>(tgt * 4));
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

    if(instr->features & INSTR_JUMP) {
        theme = THEME_JUMP;

        if(instr->uservalue & MIPS_CATEGORY_JUMPCOND)
            theme = THEME_JUMPCOND;
    }
    if(instr->features & INSTR_CALL)
        theme = THEME_CALL;
    if(instr->features & INSTR_STOP)
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
            instr->operands[1].reg = dec.instr.i_u.imm;
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
    switch(dec.opcode->category) {
        case MIPS_CATEGORY_JUMPCOND: {
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = dec.instr.i_u.rt;

            instr->operands[1].type = OP_REG;
            instr->operands[1].reg = dec.instr.i_u.rs;

            auto addr = calc_addr(dec, instr->address);
            instr->operands[2].type = OP_IMM;
            instr->operands[2].imm = addr.value_or(dec.instr.i_u.imm);

            if(addr.has_value())
                instr->operands[2].userdata1 = CR_JUMP;
            return;
        }

        case MIPS_CATEGORY_LOAD:
        case MIPS_CATEGORY_STORE: {
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = dec.instr.i_u.rt;

            instr->operands[1].type = OP_DISPL;
            instr->operands[1].displ.base = dec.instr.i_u.rs;
            instr->operands[1].displ.displ = dec.instr.i_u.imm;
            return;
        }

        default: break;
    }

    instr->operands[0].type = OP_REG;
    instr->operands[0].reg = dec.instr.i_u.rt;

    instr->operands[1].type = OP_REG;
    instr->operands[1].reg = dec.instr.i_u.rs;

    instr->operands[2].type = OP_IMM;
    instr->operands[2].imm = dec.instr.i_u.imm;
}

void decode_j(const MIPSDecodedInstruction& dec, RDInstruction* instr) {
    instr->operands[0].type = OP_IMM;

    if(auto addr = calc_addr(dec, instr->address); addr) {
        instr->operands[0].imm = *addr;
        instr->operands[0].userdata1 = CR_CALL;
    }
    else
        instr->operands[0].imm = dec.instr.j.target;
}

void decode_b(const MIPSDecodedInstruction& dec, RDInstruction* instr) {
    if(dec.opcode->id == MIPS_INSTR_BREAK)
        instr->features |= INSTR_STOP;
}

template<bool BigEndian>
void decode(const RDProcessor*, RDInstruction* instr) {
    MIPSDecodedInstruction dec;
    if(!mips_decoder::decode(instr->address, dec, BigEndian))
        return;

    instr->id = dec.opcode->id;
    instr->length = dec.length;
    instr->uservalue = dec.opcode->category;

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
        case MIPS_CATEGORY_CALL: instr->features |= INSTR_CALL; break;

        case MIPS_CATEGORY_JUMP:
            instr->features |= INSTR_JUMP | INSTR_STOP;
            break;

        case MIPS_CATEGORY_JUMPCOND: instr->features |= INSTR_JUMP; break;
        case MIPS_CATEGORY_RET: instr->features |= INSTR_STOP; break;
        default: break;
    }
}

void emulate(const RDProcessor*, RDEmulator* e, const RDInstruction* instr) {
    foreach_operand(i, op, instr) {
        switch(op->type) {
            case OP_IMM: {
                if(op->userdata1)
                    rdemulator_addref(e, op->imm, op->userdata1);
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

    switch(instr->id) {
        case MIPS_INSTR_JAL: rdrenderer_addr(r, instr->operands[0].imm); return;

        default: break;
    }

    foreach_operand(i, op, instr) {
        if(i > 0)
            rdrenderer_text(r, ", ");

        switch(op->type) {
            case OP_REG: {
                rdrenderer_reg(r, mips_decoder::reg(op->reg));
                break;
            }

            case OP_IMM: {
                if(op->userdata1)
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
