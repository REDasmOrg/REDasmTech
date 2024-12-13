#include "mips_macrodecoder.h"
#include "mips_decoder.h"
#include "mips_registers.h"
#include <string_view>
#include <unordered_map>

namespace mips_macrodecoder {

namespace {

void apply_macro(std::string_view mnemonic, MIPSDecodedInstruction& dec) {
    const auto& [macro, size] = MIPS_OPCODES_MACRO.at(mnemonic);
    dec.opcode = &macro;
    dec.length = size;
}

bool can_simplify_lui(const MIPSDecodedInstruction& lui,
                      const MIPSDecodedInstruction& nextdec) {
    switch(nextdec.opcode->format) {
        case MIPS_FORMAT_I: return lui.instr.i_u.rt == nextdec.instr.i_u.rs;

        case MIPS_FORMAT_R: {
            if(nextdec.instr.r.rd != nextdec.instr.r.rs)
                return false;
            return (lui.instr.i_u.rt == MIPS_REG_AT) &&
                   (nextdec.instr.r.rd == MIPS_REG_AT);
        }

        default: break;
    }

    return false;
}

void check_lui(RDAddress address, MIPSDecodedInstruction& lui, bool big) {
    static const std::unordered_map<int, std::string_view> LUI_MACROS = {
        {MIPS_INSTR_ADDIU, "la"}, {MIPS_INSTR_ORI, "la"},
        {MIPS_INSTR_LHU, "lhu"},  {MIPS_INSTR_LW, "lw"},
        {MIPS_INSTR_SW, "sw"},    {MIPS_INSTR_SH, "sh"},
    };

    MIPSDecodedInstruction nextdec;
    RDAddress nextaddress = address + lui.length;

    if(!mips_decoder::decode(nextaddress, nextdec, big, true) ||
       !mips_macrodecoder::can_simplify_lui(lui, nextdec))
        return;

    u32 mipsaddress = lui.instr.i_u.imm << 16;

    switch(nextdec.opcode->id) {
        case MIPS_INSTR_ORI:
            mipsaddress |= static_cast<u32>(nextdec.instr.i_s.imm);
            break;

        case MIPS_INSTR_ADDIU:
        case MIPS_INSTR_LW:
        case MIPS_INSTR_LHU:
        case MIPS_INSTR_SW:
        case MIPS_INSTR_SH:
            mipsaddress += mips_decoder::signext_16_32(nextdec.instr.i_s.imm);
            break;

        default: return;
    }

    lui.macro.regimm.reg = nextdec.instr.i_u.rt;
    lui.macro.regimm.address = mipsaddress;
    mips_macrodecoder::apply_macro(LUI_MACROS.at(nextdec.opcode->id), lui);
}

void check_li(MIPSDecodedInstruction& dec) {
    if(dec.instr.i_u.rs == MIPS_REG_ZERO)
        apply_macro("li", dec);
}

void check_move(MIPSDecodedInstruction& dec) {
    if(dec.instr.r.rt == MIPS_REG_ZERO)
        apply_macro("move", dec);
}

void check_nop(MIPSDecodedInstruction& dec) {
    if((dec.instr.r.rd == MIPS_REG_ZERO) && (dec.instr.r.rt == MIPS_REG_ZERO))
        apply_macro("nop", dec);
}

void check_b(MIPSDecodedInstruction& dec) {
    if(dec.instr.i_u.rt == dec.instr.i_u.rs)
        apply_macro("b", dec);
}

} // namespace

void check_macro(RDAddress address, MIPSDecodedInstruction& dec, bool big) {
    switch(dec.opcode->id) {
        case MIPS_INSTR_ORI:
        case MIPS_INSTR_ADDI:
        case MIPS_INSTR_ADDIU: mips_macrodecoder::check_li(dec); break;

        case MIPS_INSTR_ADDU: mips_macrodecoder::check_move(dec); break;
        case MIPS_INSTR_SLL: mips_macrodecoder::check_nop(dec); break;
        case MIPS_INSTR_BEQ: mips_macrodecoder::check_b(dec); break;

        case MIPS_INSTR_LUI:
            mips_macrodecoder::check_lui(address, dec, big);
            break;
        default: break;
    }
}

} // namespace mips_macrodecoder
