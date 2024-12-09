#include "mips_decoder.h"
#include "mips_macrodecoder.h"
#include "mips_registers.h"
#include <string>

namespace mips_decoder {

namespace {

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
    int f = mips_decoder::check_format(dec.instruction);

    switch(f) {
        case MIPS_ENCODING_R: {
            auto& format = mips_opcodes_r[dec.instruction.r.funct];
            if(!format.mnemonic)
                return false;
            dec.opcode = &format;
            break;
        }

        case MIPS_ENCODING_C: {
            auto& format = mips_opcodes_c[dec.instruction.c.funct];
            if(!format.mnemonic)
                return false;
            dec.opcode = &format;
            break;
        }

        case MIPS_ENCODING_I: {
            auto& format = mips_opcodes_i[dec.instruction.i_u.op];
            if(!format.mnemonic)
                return false;
            dec.opcode = &format;
            break;
        }

        case MIPS_ENCODING_J: {
            auto& format = mips_opcodes_j[dec.instruction.j.op];
            if(!format.mnemonic)
                return false;
            dec.opcode = &format;
            break;
        }

        case MIPS_ENCODING_B: {
            auto& format = mips_opcodes_b[dec.instruction.b.funct];
            if(!format.mnemonic)
                return false;
            dec.opcode = &format;
            break;
        }

        case MIPS_ENCODING_C0: {
            auto& format = mips_opcodes_c0[dec.instruction.c0sel.code];
            if(!format.mnemonic)
                return false;
            dec.opcode = &format;
            break;
        }

        case MIPS_ENCODING_C2: {
            auto& format = mips_opcodes_c2[dec.instruction.c2impl.code];
            if(!format.mnemonic)
                return false;
            dec.opcode = &format;
            break;
        }

        case MIPS_ENCODING_CLS: {
            auto& format = mips_opcodes_cls[dec.instruction.cls.op];
            if(!format.mnemonic)
                return false;
            dec.opcode = &format;
            break;
        }

        default:
            dec.instruction = {};
            dec.opcode = nullptr;
            return false;
    }

    return f != MIPS_ENCODING_NONE;
}

} // namespace

std::string_view reg(u32 r) {
    if(r >= GPR_REGS.size())
        return {};
    return GPR_REGS[r];
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
    bool ok = big ? rd_getu32be(address, &dec.instruction.word)
                  : rd_getu32(address, &dec.instruction.word);
    if(!ok)
        return false;

    if(mips_decoder::check_encoding(dec) && !one)
        mips_macrodecoder::check_macro(address, dec, big);

    return true;
}

} // namespace mips_decoder
