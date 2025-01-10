#include "dalvik.h"
#include "dalvik_types.h"
#include "demangler.h"
#include "dex.h"
#include <array>
#include <libdex/DexProto.h>
#include <libdex/InstrUtils.h>
#include <optional>
#include <string>
#include <unordered_map>

namespace dalvik {

namespace {

using DalvikMaxBuffer = std::array<dex_u2, 5>; // Max instruction is 5 words

enum DalvikOperands {
    DVKOP_METHODINDEX = OP_USERBASE,
    DVKOP_TYPEINDEX,
    DVKOP_STRINGINDEX,
    DVKOP_FIELDINDEX,
};

void render_integer(RDRenderer* r, const RDOperand* op) {
    if(op->dtype.id == TID_I8)
        rdrenderer_i8(r, op->s_imm, 16);
    else if(op->dtype.id == TID_I16)
        rdrenderer_i16(r, op->s_imm, 16);
    else if(op->dtype.id == TID_U32)
        rdrenderer_u32(r, op->imm, 16);
    else
        rdrenderer_cnst(r, op->imm);
}

void get_index_op(const DecodedInstruction& di, RDOperand& op) {
    switch(dexGetFormatFromOpcode(di.opcode)) {
        case kFmt20bc:
        case kFmt21c:
        case kFmt3rc:
        case kFmt3rms:
        case kFmt3rmi:
        case kFmt35c:
        case kFmt35mi:
        case kFmt35ms:
            op.user.val1 = di.vB;
            op.user.val3 = 4;
            break;

        case kFmt31c:
            op.user.val1 = di.vB;
            op.user.val3 = 8;
            break;

        case kFmt22c:
        case kFmt22cs:
            op.user.val1 = di.vC;
            op.user.val3 = 4;
            break;

        case kFmt45cc:
        case kFmt4rcc:
            op.user.val1 = di.vB;     // method index
            op.user.val2 = di.arg[4]; // proto index
            op.user.val3 = 4;         // width
            break;

        default:
            op.user.val1 = 0;
            op.user.val3 = 4;
            break;
    }
}

bool create_index_op(const DecodedInstruction& di, RDOperand& op) {
    switch(di.indexType) {
        case kIndexMethodRef: op.type = DVKOP_METHODINDEX; break;
        case kIndexTypeRef: op.type = DVKOP_TYPEINDEX; break;
        case kIndexStringRef: op.type = DVKOP_STRINGINDEX; break;
        case kIndexFieldRef: op.type = DVKOP_FIELDINDEX; break;
        default: return false;
    }

    dalvik::get_index_op(di, op);
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

void register_types(RDProcessor*, RDEmulator*) {
    rd_createstruct("PACKED_SWITCH_PAYLOAD", dalvik::PACKEDSWITCHPAYLOAD_TYPE);
    rd_createstruct("SPARSE_SWITCH_PAYLOAD", dalvik::SPARSESWITCHPAYLOAD_TYPE);
    rd_createstruct("FILL_ARRAY_DATA", dalvik::FILLARRAYDATA_TYPE);
}

const char* get_registername(const RDProcessor*, int regid) {
    static std::string reg;
    reg = "v" + std::to_string(regid);
    return reg.c_str();
}

} // namespace

struct DalvikProcessor {
    void decode(RDInstruction* instr) {
        DalvikMaxBuffer buff;
        usize n =
            rd_read(instr->address, buff.data(), buff.size() * sizeof(dex_u2));

        DecodedInstruction di;
        if(n < sizeof(dex_u2) || !dexDecodeInstruction(buff.data(), &di))
            return;

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
                    instr->address +
                    (static_cast<dex_s2>(di.vB) * sizeof(dex_u2));
                return;

            case OP_IF_LE:
            case OP_IF_LT:
                instr->features |= IF_JUMP;
                instr->operands[0].type = OP_MEM;
                instr->operands[0].mem =
                    instr->address +
                    (static_cast<dex_s2>(di.vC) * sizeof(dex_u2));
                return;

            case OP_GOTO:
                instr->features |= IF_JUMP | IF_STOP;
                instr->operands[0].type = OP_MEM;
                instr->operands[0].mem =
                    instr->address +
                    (static_cast<dex_s2>(di.vA) * sizeof(dex_u2));
                return;

            case OP_PACKED_SWITCH:
            case OP_SPARSE_SWITCH: instr->features |= IF_JUMP; break;

            default: break;
        }

        InstructionFormat fmt = dexGetFormatFromOpcode(di.opcode);

        switch(fmt) {
            case kFmt11n:
                instr->operands[0].type = OP_REG;
                instr->operands[0].reg = di.vB;
                instr->operands[1].type = OP_IMM;
                instr->operands[1].dtype.id = TID_I8; // FIXME: signed 4-bits
                instr->operands[1].s_imm = di.vB;
                break;

            case kFmt11x:
                instr->operands[0].type = OP_REG;
                instr->operands[0].reg = di.vA;
                break;

            case kFmt12x:
            case kFmt32x:
                instr->operands[0].type = OP_REG;
                instr->operands[0].reg = di.vA;
                instr->operands[1].type = OP_REG;
                instr->operands[1].reg = di.vB;
                break;

            case kFmt21h:
                instr->operands[0].type = OP_REG;
                instr->operands[0].reg = di.vA;
                instr->operands[1].type = OP_IMM;
                instr->operands[1].dtype.id = TID_I16; // FIXME: OP_CONST_HIGH16
                instr->operands[1].imm = di.vB;
                break;

            case kFmt21c:
            case kFmt31c:
                instr->operands[0].type = OP_REG;
                instr->operands[0].reg = di.vA;
                dalvik::create_index_op(di, instr->operands[1]);
                break;

            case kFmt21s:
                instr->operands[0].type = OP_REG;
                instr->operands[0].reg = di.vA;
                instr->operands[1].type = OP_IMM;
                instr->operands[1].dtype.id = TID_I16;
                instr->operands[1].s_imm = di.vB;
                break;

            case kFmt22c:
            case kFmt22cs:
                instr->operands[0].type = OP_REG;
                instr->operands[0].reg = di.vA;
                instr->operands[1].type = OP_REG;
                instr->operands[1].reg = di.vB;
                dalvik::create_index_op(di, instr->operands[2]);
                break;

            case kFmt23x:
                instr->operands[0].type = OP_REG;
                instr->operands[0].reg = di.vA;
                instr->operands[1].type = OP_REG;
                instr->operands[1].reg = di.vB;
                instr->operands[2].type = OP_REG;
                instr->operands[2].reg = di.vC;
                break;

            case kFmt31i:
                instr->operands[0].type = OP_REG;
                instr->operands[0].reg = di.vA;
                instr->operands[1].type = OP_IMM;
                instr->operands[1].dtype.id = TID_U32;
                instr->operands[1].imm = di.vB;
                break;

            case kFmt31t:
                instr->operands[0].type = OP_REG;
                instr->operands[0].reg = di.vA;
                instr->operands[1].type = OP_ADDR;
                instr->operands[1].addr =
                    instr->address + (di.vB * sizeof(dex_u2));
                break;

            case kFmt35c:
            case kFmt35mi:
            case kFmt35ms: {
                dex_u4 i = 0, n = std::min<dex_u4>(di.vA, RD_NOPERANDS);

                for(; i < n; i++) {
                    if(i == 0) instr->operands[i].userdata1 = true;
                    if(i == n - 1) instr->operands[i].userdata2 = true;

                    instr->operands[i].type = OP_REG;
                    instr->operands[i].reg = di.arg[i];
                }

                if(di.vA < RD_NOPERANDS)
                    dalvik::create_index_op(di, instr->operands[i]);
                else
                    rd_addproblem(instr->address, "Operands out of bounds");
                break;
            }

            default: break;
        }
    }

    void emulate(RDEmulator* e, const RDInstruction* instr) {
        switch(instr->id) {
            case OP_PACKED_SWITCH: {
                const auto* l =
                    reinterpret_cast<const dex::DexLoader*>(rd_getloader());
                const auto* t = l->get_pointer<PackedSwitchPayload>(
                    instr->operands[1].addr);

                rdemulator_addref(e, instr->operands[1].addr, DR_ADDRESS);
                const i32* targets = dalvik::create_packedswitchpayload(
                    instr->operands[1].addr, t);

                if(targets) {
                    for(size_t i = 0; i < t->size; i++) {
                        rdemulator_addref(
                            e, instr->address + (targets[i] * sizeof(dex_u2)),
                            CR_JUMP);
                    }
                }
                break;
            }

            case OP_SPARSE_SWITCH: {
                const auto* l =
                    reinterpret_cast<const dex::DexLoader*>(rd_getloader());
                const auto* t = l->get_pointer<SparseSwitchPayload>(
                    instr->operands[1].addr);

                rdemulator_addref(e, instr->operands[1].addr, DR_ADDRESS);
                const i32* targets = dalvik::create_sparseswitchpayload(
                    instr->operands[1].addr, t);

                if(targets) {
                    for(size_t i = 0; i < t->size; i++) {
                        rdemulator_addref(
                            e, instr->address + (targets[i] * sizeof(dex_u2)),
                            CR_JUMP);
                    }
                }
                break;
            }

            case OP_FILL_ARRAY_DATA: {
                const auto* l =
                    reinterpret_cast<const dex::DexLoader*>(rd_getloader());

                const auto* t =
                    l->get_pointer<FillArrayData>(instr->operands[1].addr);
                rdemulator_addref(e, instr->operands[1].addr, DR_ADDRESS);
                dalvik::create_fillarraydata(instr->operands[1].addr, t);
                break;
            }

            default: {
                if(instr->features & IF_JUMP)
                    rdemulator_addref(e, instr->operands[0].mem, CR_JUMP);
                break;
            }
        }

        foreach_operand(i, op, instr) {
            if(op->type == DVKOP_STRINGINDEX) {
                auto addr = this->get_stringaddr(op->user.val1);
                if(addr) {
                    rdemulator_addref(e, *addr, DR_ADDRESS);
                    rd_settypename(*addr, "str", nullptr);
                }
            }
        }

        if(!(instr->features & IF_STOP))
            rdemulator_flow(e, instr->address + instr->length);
    }

    void render_instruction(RDRenderer* r, const RDInstruction* instr) const {
        rdrenderer_mnem(r, dexGetOpcodeName(static_cast<Opcode>(instr->id)),
                        dalvik::get_op_theme(instr->id));

        foreach_operand(i, op, instr) {
            if(i) rdrenderer_text(r, ",");

            switch(op->type) {
                case OP_REG: {
                    if(op->userdata1) rdrenderer_text(r, "{");
                    rdrenderer_reg(r, op->reg);
                    if(op->userdata2) rdrenderer_text(r, "}");
                    break;
                }

                case OP_ADDR: {
                    rdrenderer_addr(r, op->addr);
                    break;
                }

                case OP_MEM: rdrenderer_addr(r, op->mem); break;
                case OP_IMM: dalvik::render_integer(r, op); break;

                case DVKOP_TYPEINDEX: {
                    if(auto type = this->get_typename(op->user.val1); type)
                        rdrenderer_themed(r, type->c_str(), THEME_TYPE);
                    else
                        rdrenderer_unkn(r);
                    break;
                }

                case DVKOP_STRINGINDEX: {
                    if(auto addr = this->get_stringaddr(op->user.val1); addr)
                        rdrenderer_addr(r, *addr);
                    else
                        rdrenderer_unkn(r);
                    break;
                }

                case DVKOP_FIELDINDEX: {
                    if(auto field = this->get_fieldname(op->user.val1); field)
                        rdrenderer_themed(r, field->c_str(), THEME_DATA);
                    else
                        rdrenderer_unkn(r);
                    break;
                }

                case DVKOP_METHODINDEX: {
                    if(auto meth = this->get_methodname(op->user.val1); meth)
                        rdrenderer_themed(r, meth->c_str(), THEME_FUNCTION);
                    else
                        rdrenderer_unkn(r);
                    break;
                }

                default: rdrenderer_unkn(r); break;
            }
        }
    }

private:
    std::optional<RDAddress> get_stringaddr(dex_u4 idx) const {
        const auto* l = reinterpret_cast<const dex::DexLoader*>(rd_getloader());
        return l->get_address(dexStringById(l->dexfile, idx));
    }

    std::optional<std::string> get_typename(dex_u4 idx) const {
        const auto* l = reinterpret_cast<const dex::DexLoader*>(rd_getloader());
        const char* s = dexStringByTypeIdx(l->dexfile, idx);
        if(s) return demangler::parse(s);
        return std::nullopt;
    }

    std::optional<std::string> get_fieldname(dex_u4 idx) const {
        const auto* l = reinterpret_cast<const dex::DexLoader*>(rd_getloader());
        auto it = m_fieldcache.find(idx);
        if(it != m_fieldcache.end()) return it->second;

        const DexFieldId* fieldid = dexGetFieldId(l->dexfile, idx);
        if(!fieldid) return std::nullopt;

        const char* name = dexStringById(l->dexfile, fieldid->nameIdx);
        if(!name) return std::nullopt;

        const char* signature =
            dexStringByTypeIdx(l->dexfile, fieldid->typeIdx);
        if(!signature) return std::nullopt;

        const auto* classdescr =
            dexStringByTypeIdx(l->dexfile, fieldid->classIdx);
        if(!classdescr) return std::nullopt;

        m_fieldcache[idx] = demangler::get_objectname(classdescr) + "." + name +
                            demangler::get_signature(signature);
        return m_fieldcache[idx];
    }

    std::optional<std::string> get_methodname(dex_u4 idx) const {
        const auto* l = reinterpret_cast<const dex::DexLoader*>(rd_getloader());
        auto it = m_methodcache.find(idx);
        if(it != m_methodcache.end()) return it->second;

        const DexMethodId* methodid = dexGetMethodId(l->dexfile, idx);
        if(!methodid) return std::nullopt;

        const char* name = dexStringById(l->dexfile, methodid->nameIdx);
        if(!name) return std::nullopt;

        const char* classdescr =
            dexStringByTypeIdx(l->dexfile, methodid->classIdx);
        if(!classdescr) return std::nullopt;

        char* signature = dexCopyDescriptorFromMethodId(l->dexfile, methodid);
        m_methodcache[idx] = demangler::get_objectname(classdescr) + "." +
                             name + demangler::get_signature(signature);
        std::free(signature);
        return m_methodcache.at(idx);
    }

private:
    mutable std::unordered_map<u16, std::string> m_methodcache, m_fieldcache;
    mutable std::unordered_map<RDAddress, std::string> m_functioncache;
};

// clang-format off
RDProcessorPlugin processor = {
    .id = "dalvik",
    .name = "Dalvik VM",
    .create = [](const RDProcessorPlugin*) { 
        return reinterpret_cast<RDProcessor*>(new DalvikProcessor{});
    },
    .destroy = [](RDProcessor* self) { 
        delete reinterpret_cast<DalvikProcessor*>(self);
    },
    .address_size = 2,
    .integer_size = 2,
    .setup = dalvik::register_types,
    .getregistername = dalvik::get_registername,
    .decode = [](RDProcessor* self, RDInstruction* instr) { 
        reinterpret_cast<DalvikProcessor*>(self)->decode(instr);
    },
    .emulate = [](RDProcessor* self, RDEmulator* e, const RDInstruction* instr) { 
        reinterpret_cast<DalvikProcessor*>(self)->emulate(e, instr);
    },
    .renderinstruction = [](const RDProcessor* self, RDRenderer* r, const RDInstruction* instr) { 
        reinterpret_cast<const DalvikProcessor*>(self)->render_instruction(r, instr);
    },
};

// clang-format on

} // namespace dalvik
