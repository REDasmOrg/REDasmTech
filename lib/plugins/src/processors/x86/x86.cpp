#include "x86.h"
#include "x86_common.h"
#include "x86_lifter.h"

namespace {

void apply_optype(const ZydisDecodedOperand& zop, RDOperand& op) {
    if(!zop.element_size) {
        op.dtype = {};
        return;
    }

    switch(zop.element_type) {
        case ZYDIS_ELEMENT_TYPE_INT: {
            if(zop.element_size == 8)
                op.dtype.id = TID_I8;
            else if(zop.element_size == 16)
                op.dtype.id = TID_I16;
            else if(zop.element_size == 32)
                op.dtype.id = TID_I32;
            else if(zop.element_size == 64)
                op.dtype.id = TID_I64;
            else
                op.dtype.id = 0;
            break;
        }

        case ZYDIS_ELEMENT_TYPE_UINT: {
            if(zop.element_size == 8)
                op.dtype.id = TID_U8;
            else if(zop.element_size == 16)
                op.dtype.id = TID_U16;
            else if(zop.element_size == 32)
                op.dtype.id = TID_U32;
            else if(zop.element_size == 64)
                op.dtype.id = TID_U64;
            else
                op.dtype.id = 0;
            break;
        }

        default: op.dtype = {}; return;
    }

    if(op.dtype.id && zop.element_count > 1)
        op.dtype.n = zop.element_count;
}

bool is_branch_instruction(const RDInstruction* instr) {
    switch(instr->uservalue) {
        case ZYDIS_CATEGORY_UNCOND_BR:
        case ZYDIS_CATEGORY_COND_BR:
        case ZYDIS_CATEGORY_CALL: return true;

        default: break;
    }

    return false;
}

bool is_addr_instruction(const RDInstruction* instr) {
    switch(instr->id) {
        case ZYDIS_MNEMONIC_PUSH:
        case ZYDIS_MNEMONIC_MOV: return true;

        default: break;
    }

    return false;
}

} // namespace

X86Processor::X86Processor(usize bits) {
    if(bits == 32) {
        ZydisDecoderInit(&m_decoder, ZYDIS_MACHINE_MODE_LEGACY_32,
                         ZYDIS_STACK_WIDTH_32);
    }
    else {
        ZydisDecoderInit(&m_decoder, ZYDIS_MACHINE_MODE_LONG_64,
                         ZYDIS_STACK_WIDTH_64);
    }
}

void X86Processor::decode(RDInstruction* instr) {
    usize n = rd_memoryread(instr->address, m_dbuffer.data(), m_dbuffer.size());

    ZydisDecodedInstruction zinstr;
    std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT> zops;

    if(!n || !ZYAN_SUCCESS(ZydisDecoderDecodeFull(&m_decoder, m_dbuffer.data(),
                                                  n, &zinstr, zops.data()))) {
        return;
    }

    instr->id = zinstr.mnemonic;
    instr->length = zinstr.length;
    instr->uservalue = zinstr.meta.category;

    switch(zinstr.meta.category) {
        case ZYDIS_CATEGORY_CALL: instr->features |= IF_CALL; break;
        case ZYDIS_CATEGORY_COND_BR: instr->features |= IF_JUMP; break;

        case ZYDIS_CATEGORY_UNCOND_BR:
            instr->features |= IF_JUMP | IF_STOP;
            break;

        default: break;
    };

    switch(zinstr.mnemonic) {
        case ZYDIS_MNEMONIC_HLT:
        case ZYDIS_MNEMONIC_INT3:
        case ZYDIS_MNEMONIC_RET: instr->features |= IF_STOP; break;
        default: break;
    }

    for(auto i = 0, j = 0; i < zinstr.operand_count; i++) {
        const ZydisDecodedOperand& zop = zops[i];
        if(zop.visibility == ZYDIS_OPERAND_VISIBILITY_HIDDEN)
            continue;

        RDOperand& op = instr->operands[j++];
        apply_optype(zop, op);

        switch(zop.type) {
            case ZYDIS_OPERAND_TYPE_REGISTER:
                op.type = OP_REG;
                op.reg = zop.reg.value;
                break;

            case ZYDIS_OPERAND_TYPE_IMMEDIATE: {
                ZyanU64 addr = 0;

                if(is_branch_instruction(instr) &&
                   ZYAN_SUCCESS(ZydisCalcAbsoluteAddress(
                       &zinstr, &zop, instr->address, &addr))) {
                    op.type = OP_ADDR;
                    op.addr = addr;
                }
                else if(is_addr_instruction(instr) &&
                        rd_isaddress(zop.imm.value.u)) {
                    op.type = OP_ADDR;
                    op.addr = zop.imm.value.u;
                }
                else {
                    op.type = OP_IMM;
                    op.imm = zop.imm.value.u;
                }
                break;
            }

            case ZYDIS_OPERAND_TYPE_MEMORY: {
                if(zop.mem.base != ZYDIS_REGISTER_NONE &&
                   zop.mem.index != ZYDIS_REGISTER_NONE &&
                   !zop.mem.disp.has_displacement) {
                    op.type = OP_PHRASE;
                    op.phrase.base = zop.mem.base;
                    op.phrase.index = zop.mem.index;
                    op.userdata1 = zop.mem.segment;
                }
                else if(zop.mem.base == ZYDIS_REGISTER_NONE &&
                        zop.mem.index == ZYDIS_REGISTER_NONE) {
                    op.type = OP_MEM;

                    ZyanU64 addr;

                    if((instr->features & IF_JUMP ||
                        instr->features & IF_CALL) &&
                       ZYAN_SUCCESS(ZydisCalcAbsoluteAddress(
                           &zinstr, &zop, instr->address, &addr))) {
                        op.mem = addr;
                    }
                    op.mem = zop.mem.disp.value;
                    op.userdata1 = zop.mem.segment;
                }
                else {
                    op.type = OP_DISPL;
                    op.displ.base = zop.mem.base;
                    op.displ.index = zop.mem.index;
                    op.displ.scale = zop.mem.scale;
                    op.displ.displ = zop.mem.disp.value;
                    op.userdata1 = zop.mem.segment;
                }
                break;
            }

            default: break;
        }
    }
}

namespace {

void render_instruction(const RDProcessor* /*self*/, RDRenderer* r,
                        const RDInstruction* instr) {
    const char* mnemonic =
        ZydisMnemonicGetString(static_cast<ZydisMnemonic>(instr->id));

    switch(instr->uservalue) {
        case ZYDIS_CATEGORY_COND_BR:
            rdrenderer_mnem(r, mnemonic, THEME_JUMPCOND);
            break;
        case ZYDIS_CATEGORY_UNCOND_BR:
            rdrenderer_mnem(r, mnemonic, THEME_JUMP);
            break;
        case ZYDIS_CATEGORY_CALL:
            rdrenderer_mnem(r, mnemonic, THEME_CALL);
            break;
        case ZYDIS_CATEGORY_RET: rdrenderer_mnem(r, mnemonic, THEME_RET); break;
        case ZYDIS_CATEGORY_NOP: rdrenderer_mnem(r, mnemonic, THEME_NOP); break;
        default: rdrenderer_mnem(r, mnemonic, THEME_DEFAULT); break;
    }

    foreach_operand(i, op, instr) {
        if(i > 0)
            rdrenderer_text(r, ", ");

        switch(op->type) {
            case OP_ADDR: rdrenderer_addr(r, op->addr); break;
            case OP_IMM: rdrenderer_cnst(r, op->imm); break;

            case OP_MEM: {
                rdrenderer_text(r, "[");

                if(op->userdata1 && op->userdata1 != ZYDIS_REGISTER_CS &&
                   op->userdata1 != ZYDIS_REGISTER_DS) {
                    rdrenderer_reg(r, op->userdata1);
                    rdrenderer_text(r, ":");
                }

                rdrenderer_addr(r, op->mem);
                rdrenderer_text(r, "]");
                break;
            }

            case OP_REG: rdrenderer_reg(r, op->reg); break;

            case OP_PHRASE: {
                rdrenderer_text(r, "[");
                rdrenderer_reg(r, op->phrase.base);
                rdrenderer_text(r, "+");
                rdrenderer_reg(r, op->phrase.index);
                rdrenderer_text(r, "]");
                break;
            }

            case OP_DISPL: {
                rdrenderer_text(r, "[");
                rdrenderer_reg(r, op->displ.base);

                if(op->displ.index != ZYDIS_REGISTER_NONE) {
                    rdrenderer_text(r, "+");
                    rdrenderer_reg(r, op->displ.index);

                    if(op->displ.scale > 1) {
                        rdrenderer_text(r, "*");
                        rdrenderer_cnst(r, op->displ.scale);
                    }
                }

                if(op->displ.displ != 0) {
                    rdrenderer_addr_ex(r, op->displ.displ, RC_NEEDSIGN);
                }

                rdrenderer_text(r, "]");
                break;
            }

            default: break;
        }
    }
}

void decode(const RDProcessor* self, RDInstruction* instr) {
    reinterpret_cast<X86Processor*>(self->userdata)->decode(instr);
}

void emulate(const RDProcessor* /*self*/, RDEmulator* e,
             const RDInstruction* instr) {
    foreach_operand(i, op, instr) {
        switch(op->type) {
            case OP_ADDR: {
                if(instr->features & IF_JUMP)
                    rdemulator_addref(e, op->addr, CR_JUMP);
                else if(instr->features & IF_CALL)
                    rdemulator_addref(e, op->addr, CR_CALL);
                else
                    rdemulator_addref(e, op->addr, DR_ADDRESS);
                break;
            }

            case OP_MEM: {
                if(instr->features & IF_JUMP) {
                    auto addr = x86_common::read_address(op->mem);
                    if(addr)
                        rdemulator_addref(e, *addr, CR_JUMP);
                }
                else if(instr->features & IF_CALL) {
                    auto addr = x86_common::read_address(op->mem);
                    if(addr)
                        rdemulator_addref(e, *addr, CR_CALL);
                }

                rdemulator_addref(e, op->mem, DR_READ);
                break;
            }

            default: break;
        }
    }

    if(!(instr->features & IF_STOP))
        rdemulator_flow(e, instr->address + instr->length);
}

const char* get_register_name(const RDProcessor*, int reg) {
    return ZydisRegisterGetString(static_cast<ZydisRegister>(reg));
}

RDProcessor x86_32{};
RDProcessor x86_64{};

} // namespace

void rdplugin_init() {
    x86_32.id = "x86_32";
    x86_32.name = "X86";
    x86_32.address_size = 4;
    x86_32.integer_size = 4;
    x86_32.userdata = new X86Processor(32);
    x86_32.getregistername = get_register_name;
    x86_32.renderinstruction = render_instruction;
    x86_32.decode = decode;
    x86_32.emulate = emulate;
    x86_32.lift = x86_lifter::lift;
    rd_registerprocessor(&x86_32);

    x86_64.id = "x86_64";
    x86_64.name = "X64";
    x86_64.address_size = 8;
    x86_64.integer_size = 4;
    x86_64.userdata = new X86Processor(64);
    x86_64.getregistername = get_register_name;
    x86_64.renderinstruction = render_instruction;
    x86_64.decode = decode;
    x86_64.emulate = emulate;
    x86_64.lift = x86_lifter::lift;
    rd_registerprocessor(&x86_64);
}

void rdplugin_free() {
    if(x86_32.userdata)
        delete reinterpret_cast<X86Processor*>(x86_32.userdata);

    if(x86_64.userdata)
        delete reinterpret_cast<X86Processor*>(x86_64.userdata);

    x86_32.userdata = nullptr;
    x86_64.userdata = nullptr;
}
