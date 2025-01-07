#include "dalvik.h"
#include <array>
#include <libdex/InstrUtils.h>
#include <string>

namespace dalvik {

namespace {

using DalvikMaxBuffer = std::array<dex_u2, 5>; // Max instruction is 5 words

struct DalvikIndex {
    dex_u4 index{0};
    dex_u4 secindex{0};
    dex_u4 width{0};
};

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

void render_integer(RDRenderer* r, const RDOperand* op) {
    if(op->dtype.id == TID_I16)
        rdrenderer_i16(r, op->s_imm, 16);
    else
        rdrenderer_cnst(r, op->imm);
}

void get_index_op(const DecodedInstruction& di, DalvikIndex& res) {
    switch(dexGetFormatFromOpcode(di.opcode)) {
        case kFmt20bc:
        case kFmt21c:
        case kFmt35c:
        case kFmt35ms:
        case kFmt3rc:
        case kFmt3rms:
        case kFmt35mi:
        case kFmt3rmi:
            res.index = di.vB;
            res.width = 4;
            break;

        case kFmt31c:
            res.index = di.vB;
            res.width = 8;
            break;

        case kFmt22c:
        case kFmt22cs:
            res.index = di.vC;
            res.width = 4;
            break;

        case kFmt45cc:
        case kFmt4rcc:
            res.index = di.vB;        // method index
            res.secindex = di.arg[4]; // proto index
            res.width = 4;
            break;

        default:
            res.index = 0;
            res.width = 4;
            break;
    }
}

bool create_index_op(const DecodedInstruction& di, RDOperand& op) {
    DalvikIndex index;
    dalvik::get_index_op(di, index);

    switch(di.indexType) {
        case kIndexFieldRef: {
            break;
        }

        default: return false;
    }

    return true;
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

const char* get_registername(const RDProcessor*, int regid) {
    static std::string reg;
    reg = "v" + std::to_string(regid);
    return reg.c_str();
}

void decode(RDProcessor*, RDInstruction* instr) {
    if(instr->address == 0x42624) {
        int zzz = 0;
        zzz++;
    }

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

        case kFmt21c:
        case kFmt31c:
            instr->operands[0].type = OP_REG;
            instr->operands[0].reg = di.vA;
            dalvik::create_index_op(di, instr->operands[1]);
            break;

        default: break;
    }
}

void emulate(RDProcessor*, RDEmulator* e, const RDInstruction* instr) {
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

} // namespace

RDProcessorPlugin processor = {
    .id = "dalvik",
    .name = "Dalvik VM",
    .address_size = 2,
    .integer_size = 2,
    .getregistername = dalvik::get_registername,
    .decode = dalvik::decode,
    .emulate = dalvik::emulate,
    .renderinstruction = dalvik::render_instruction,
};

} // namespace dalvik
