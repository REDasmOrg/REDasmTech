#include "mips_decoder.h"
#include "mips_macrodecoder.h"
#include "mips_registers.h"
#include <array>
#include <string>
#include <string_view>

namespace mips_decoder {

namespace {

constexpr std::array<std::string_view, 85> MIPS_INSTR_MNEMONICS = {
    // R-Type
    "add", "addu", "and", "div", "divu", "mult", "multu", "nor", "or", "sll",
    "sra", "srl", "sub", "subu", "xor", "slt", "sltu", "jr", "mfhi", "mflo",
    "mthi", "mtlo", "sllv", "srav", "srlv", "xori", "jalr",

    // C-Type
    "teq", "tge",

    // I-Type
    "addi", "addiu", "andi", "ori", "lui", "beq", "bgez", "bgtz", "blez", "bne",
    "lb", "lbu", "lh", "lhu", "lw", "lwl", "lwr", "sb", "sh", "sw", "swl",
    "swr", "lhi", "llo", "slti", "sltiu",

    // J-Type
    "j", "jal",

    // B-Type
    "break", "syscall",

    // C0-Type
    "mfc0", "mtc0",

    // C2-Type
    "mfc2", "mtc2", "cfc2", "ctc2",

    // CLS-Type
    "lwc1", "swc1", "lwc2", "swc2",

    // Macro Instructions
    "la", "li", "move", "lhu", "lw", "sw", "sh", "b", "nop"};

int check_format(const MIPSInstruction& mi) {
    if(!mi.r.op) {
        switch(mi.b.funct) {
            case 0b001100:
            case 0b001101: return MIPS_ENCODING_B;

            case 0b110000:
            case 0b110100: return MIPS_ENCODING_C;
            default: break;
        }

        return MIPS_ENCODING_R;
    }

    switch(mi.unk.op) {
        case 0b010000: return MIPS_ENCODING_C0;
        // case 0b010001: return MIPSEncoding_C1;
        case 0b010010: return MIPS_ENCODING_C2;

        case 0b110001:
        case 0b111001:
        case 0b110010:
        case 0b111010: return MIPS_ENCODING_CLS;

        default: break;
    }

    if(((mi.i_u.op >= 0x04) && (mi.i_u.op <= 0x2e)) || (mi.i_u.op == 0x01))
        return MIPS_ENCODING_I;
    if((mi.j.op == 0x02) || (mi.j.op == 0x03))
        return MIPS_ENCODING_J;

    return MIPS_ENCODING_NONE;
}

bool check_encoding(MIPSDecodedInstruction& dec) {
    int f = mips_decoder::check_format(dec.instr);

    switch(f) {
        case MIPS_ENCODING_R:
            dec.opcode = &mips_opcodes_r[dec.instr.r.funct];
            break;

        case MIPS_ENCODING_C:
            dec.opcode = &mips_opcodes_c[dec.instr.c.funct];
            break;

        case MIPS_ENCODING_I:
            dec.opcode = &mips_opcodes_i[dec.instr.i_u.op];
            break;

        case MIPS_ENCODING_J:
            dec.opcode = &mips_opcodes_j[dec.instr.j.op];
            break;

        case MIPS_ENCODING_B:
            dec.opcode = &mips_opcodes_b[dec.instr.b.funct];
            break;

        case MIPS_ENCODING_C0:
            dec.opcode = &mips_opcodes_c0[dec.instr.c0sel.code];
            break;

        case MIPS_ENCODING_C2:
            dec.opcode = &mips_opcodes_c2[dec.instr.c2impl.code];
            break;

        case MIPS_ENCODING_CLS:
            dec.opcode = &mips_opcodes_cls[dec.instr.cls.op];
            break;

        default:
            dec.instr = {};
            dec.opcode = nullptr;
            return false;
    }

    return f != MIPS_ENCODING_NONE;
}

} // namespace

const char* mnemonic(usize id) {
    if(id >= MIPS_INSTR_MNEMONICS.size())
        return "???";

    return MIPS_INSTR_MNEMONICS[id].data(); // NOLINT
}

const char* reg(u32 r) {
    if(r >= GPR_REGS.size())
        return "???";
    return GPR_REGS[r].data(); // NOLINT
}

std::string_view cop0_reg(u32 r) {
    if(r >= COP0_REGS.size())
        return {};
    return COP0_REGS[r];
}

std::string_view copn_reg(u32 r) {
    static std::string copreg;
    copreg = "$" + std::to_string(r);
    return copreg;
}

bool decode(RDAddress address, MIPSDecodedInstruction& dec, bool big,
            bool one) {
    bool ok = big ? rd_getu32be(address, &dec.instr.word)
                  : rd_getu32(address, &dec.instr.word);
    if(ok) {
        ok = mips_decoder::check_encoding(dec);

        if(ok && !one)
            mips_macrodecoder::check_macro(address, dec, big);
    }

    return ok;
}

} // namespace mips_decoder
