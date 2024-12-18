#include "mips_macrodecoder.h"
#include "mips_decoder.h"
#include "mips_registers.h"
#include <unordered_map>

namespace mips_macrodecoder {

namespace {

void apply_macro(MIPSInstructionId id, MIPSDecodedInstruction& dec) {
    const auto& [macro, size] = MIPS_OPCODES_MACRO.at(id);
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
    static const std::unordered_map<int, MIPSInstructionId> LUI_MACROS = {
        {MIPS_INSTR_ADDIU, MIPS_MACRO_LA}, {MIPS_INSTR_ORI, MIPS_MACRO_LA},
        {MIPS_INSTR_LHU, MIPS_MACRO_LHU},  {MIPS_INSTR_LW, MIPS_MACRO_LW},
        {MIPS_INSTR_SW, MIPS_MACRO_SW},    {MIPS_INSTR_SH, MIPS_MACRO_SH},
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
        apply_macro(MIPS_MACRO_LI, dec);
}

void check_addu(MIPSDecodedInstruction& dec) {
    if(dec.instr.r.rt == MIPS_REG_ZERO)
        apply_macro(MIPS_MACRO_MOVE, dec);
}

void check_sll(MIPSDecodedInstruction& dec) {
    if((dec.instr.r.rd == MIPS_REG_ZERO) && (dec.instr.r.rt == MIPS_REG_ZERO))
        apply_macro(MIPS_MACRO_NOP, dec);
}

void check_beq(MIPSDecodedInstruction& dec) {
    if(dec.instr.i_s.rt == dec.instr.i_s.rs)
        apply_macro(MIPS_MACRO_B, dec);
    else if(dec.instr.i_s.rt == MIPS_REG_ZERO ||
            dec.instr.i_s.rs == MIPS_REG_ZERO) {
        apply_macro(MIPS_MACRO_BEQZ, dec);

        if(dec.instr.i_s.rt != MIPS_REG_ZERO)
            dec.macro.regimm.reg = dec.instr.i_s.rt;
        else if(dec.instr.i_s.rs != MIPS_REG_ZERO)
            dec.macro.regimm.reg = dec.instr.i_s.rs;

        dec.macro.regimm.s_imm16 = dec.instr.i_s.imm;
    }
}

void check_bne(MIPSDecodedInstruction& dec) {
    if(dec.instr.i_s.rt == MIPS_REG_ZERO || dec.instr.i_s.rs == MIPS_REG_ZERO) {
        apply_macro(MIPS_MACRO_BNEZ, dec);

        if(dec.instr.i_s.rt != MIPS_REG_ZERO)
            dec.macro.regimm.reg = dec.instr.i_s.rt;
        else if(dec.instr.i_s.rs != MIPS_REG_ZERO)
            dec.macro.regimm.reg = dec.instr.i_s.rs;

        dec.macro.regimm.s_imm16 = dec.instr.i_s.imm;
    }
}

} // namespace

void check_macro(RDAddress address, MIPSDecodedInstruction& dec, bool big) {
    switch(dec.opcode->id) {
        case MIPS_INSTR_ORI:
        case MIPS_INSTR_ADDI:
        case MIPS_INSTR_ADDIU: mips_macrodecoder::check_li(dec); break;

        case MIPS_INSTR_ADDU: mips_macrodecoder::check_addu(dec); break;
        case MIPS_INSTR_SLL: mips_macrodecoder::check_sll(dec); break;
        case MIPS_INSTR_BEQ: mips_macrodecoder::check_beq(dec); break;
        case MIPS_INSTR_BNE: mips_macrodecoder::check_bne(dec); break;

        case MIPS_INSTR_LUI:
            mips_macrodecoder::check_lui(address, dec, big);
            break;
        default: break;
    }
}

} // namespace mips_macrodecoder
