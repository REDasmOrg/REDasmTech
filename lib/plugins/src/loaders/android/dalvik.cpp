#include "dalvik.h"
#include <array>
#include <libdex/InstrUtils.h>
#include <string>

namespace dalvik {

namespace {

void render_integer(RDRenderer* r, const RDOperand* op) {
    if(op->dtype.id == TID_I16)
        rdrenderer_i16(r, op->s_imm, 16);
    else
        rdrenderer_cnst(r, op->imm);
}

RDThemeKind get_op_theme(u32 opcode) {
    switch(opcode) {
        case OP_RETURN:
        case OP_RETURN_OBJECT:
        case OP_RETURN_VOID:
        case OP_RETURN_WIDE: return THEME_RET;

        case OP_INVOKE_DIRECT:
        case OP_INVOKE_DIRECT_RANGE:
        case OP_INVOKE_INTERFACE:
        case OP_INVOKE_INTERFACE_RANGE:
        case OP_INVOKE_STATIC:
        case OP_INVOKE_STATIC_RANGE:
        case OP_INVOKE_SUPER:
        case OP_INVOKE_SUPER_RANGE:
        case OP_INVOKE_VIRTUAL:
        case OP_INVOKE_VIRTUAL_RANGE: return THEME_CALL;

        case OP_IF_EQ:
        case OP_IF_EQZ:
        case OP_IF_GE:
        case OP_IF_GEZ:
        case OP_IF_GT:
        case OP_IF_GTZ:
        case OP_IF_LE:
        case OP_IF_LEZ:
        case OP_IF_LT:
        case OP_IF_LTZ:
        case OP_IF_NE:
        case OP_IF_NEZ:
        case OP_SPARSE_SWITCH: return THEME_JUMPCOND;

        case OP_GOTO:
        case OP_GOTO_16:
        case OP_GOTO_32:
        case OP_PACKED_SWITCH: return THEME_JUMP;

        case OP_NOP: return THEME_NOP;
        default: break;
    }

    return THEME_DEFAULT;
}

} // namespace

enum DalvikOperands {
    DVKOP_VNORMAL = 0,
    DVKOP_VMETHODINDEX,
    DVKOP_VTYPEINDEX,
    DVKOP_VSTRINGINDEX,
    DVKOP_VFIELDINDEX,
    DVKOP_VPACKEDSWITCHTABLE,
    DVKOP_VSPARSESWITCHTABLE,
    DVKOP_VFILLARRAYDATA,

    DVKOP_PARAMETERFIRST = 0x1000,
    DVKOP_PARAMETERLAST = 0x2000,
    DVKOP_PARAMETERTHIS = 0x4000
};

using DalvikMaxBuffer = std::array<dex_u2, 5>; // Max instruction is 5 words

const char* get_registername(const RDProcessor*, int regid) {
    static std::string reg;
    reg = "v" + std::to_string(regid);
    return reg.c_str();
}

void decode(const RDProcessor*, RDInstruction* instr) {
    // if(instr->address == 0x42624) {
    //     int zzz = 0;
    //     zzz++;
    // }

    DalvikMaxBuffer buff;
    usize n =
        rd_read(instr->address, buff.data(), buff.size() * sizeof(dex_u2));

    DecodedInstruction di;
    if(n < sizeof(dex_u2) || !dexDecodeInstruction(buff.data(), &di)) return;

    u32 len = dexGetWidthFromInstruction(buff.data()) * sizeof(dex_u2);
    if(!len || len > n) return;

    instr->id = di.opcode;
    instr->length = len;

    switch(di.opcode) {
        case OP_RETURN_VOID:
        case OP_RETURN:
        case OP_RETURN_WIDE:
        case OP_RETURN_OBJECT: instr->features |= IF_STOP; return;

        case OP_IF_LEZ:
        case OP_IF_EQZ:
            instr->features |= IF_JUMP;
            instr->operands[0].type = OP_MEM;
            instr->operands[0].mem =
                instr->address + (static_cast<dex_s2>(di.vB) * sizeof(dex_u2));
            return;

        case OP_IF_LE:
        case OP_IF_LT:
            instr->features |= IF_JUMP;
            instr->operands[0].type = OP_MEM;
            instr->operands[0].mem =
                instr->address + (static_cast<dex_s2>(di.vC) * sizeof(dex_u2));
            return;

        case OP_GOTO:
            instr->features |= IF_JUMP | IF_STOP;
            instr->operands[0].type = OP_MEM;
            instr->operands[0].mem =
                instr->address + (static_cast<dex_s2>(di.vA) * sizeof(dex_u2));
            return;

        default: break;
    }

    InstructionFormat fmt = dexGetFormatFromOpcode(di.opcode);

    switch(fmt) {
        case kFmt21s:
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = di.vA;
            instr->operands[1].type = OP_IMM;
            instr->operands[1].dtype.id = TID_I16;
            instr->operands[1].s_imm = di.vB;
            break;

        default: break;
    }
}

void emulate(const RDProcessor*, RDEmulator* e, const RDInstruction* instr) {
    if(instr->features & IF_JUMP)
        rdemulator_addref(e, instr->operands[0].mem, CR_JUMP);

    if(!(instr->features & IF_STOP))
        rdemulator_flow(e, instr->address + instr->length);
}

void render_instruction(const RDProcessor*, RDRenderer* r,
                        const RDInstruction* instr) {

    rdrenderer_mnem(r, dexGetOpcodeName(static_cast<Opcode>(instr->id)),
                    dalvik::get_op_theme(instr->id));

    foreach_operand(i, op, instr) {
        if(i) rdrenderer_text(r, ",");

        switch(op->type) {
            case OP_REG: rdrenderer_reg(r, op->reg); break;
            case OP_MEM: rdrenderer_addr(r, op->mem); break;
            case OP_IMM: dalvik::render_integer(r, op); break;
            default: rdrenderer_unkn(r); break;
        }
    }
}

} // namespace dalvik
