#include "dalvik.h"
#include <array>
#include <libdex/InstrUtils.h>

namespace dalvik {

namespace {

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

using DalvikMaxBuffer = std::array<u16, 5>; // Max instruction is 5 words

void decode(const RDProcessor*, RDInstruction* instr) {
    DalvikMaxBuffer buff;
    usize n = rd_read(instr->address, buff.data(), buff.size());

    DecodedInstruction di;
    if(n < sizeof(u16) || !dexDecodeInstruction(buff.data(), &di)) return;

    u32 len = dexGetWidthFromInstruction(buff.data()) * sizeof(u16);
    if(!len || len > n) return;

    instr->id = di.opcode;
    instr->length = len;

    switch(instr->id) {
        case OP_RETURN_VOID:
        case OP_RETURN:
        case OP_RETURN_WIDE:
        case OP_RETURN_OBJECT: instr->features |= IF_STOP; break;

        default: break;
    }
}

void emulate(const RDProcessor*, RDEmulator* e, const RDInstruction* instr) {
    if(!(instr->features & IF_STOP))
        rdemulator_flow(e, instr->address + instr->length);
}

void render_instruction(const RDProcessor*, RDRenderer* r,
                        const RDInstruction* instr) {

    rdrenderer_mnem(r, dexGetOpcodeName(static_cast<Opcode>(instr->id)),
                    dalvik::get_op_theme(instr->id));
}

} // namespace dalvik
