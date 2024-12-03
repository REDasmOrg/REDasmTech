#include "x86.h"
#include "x86_common.h"
#include "x86_lifter.h"
#include <string>

namespace {

void apply_optype(const ZydisDecodedOperand& zop, RDOperand& op) {
    if(!zop.element_size) {
        op.dtype = {};
        return;
    }

    std::string tname;

    switch(zop.element_type) {
        case ZYDIS_ELEMENT_TYPE_UINT: tname = "u"; break;
        case ZYDIS_ELEMENT_TYPE_INT: tname = "i"; break;
        default: op.dtype = {}; return;
    }

    tname += std::to_string(zop.element_size);

    if(zop.element_count > 1)
        tname += '[' + std::to_string(zop.element_count) + ']';

    rd_createtype(tname.c_str(), &op.dtype);
}

bool is_addr_instruction(const RDInstruction* instr) {
    switch(instr->uservalue) {
        case ZYDIS_CATEGORY_UNCOND_BR:
        case ZYDIS_CATEGORY_COND_BR:
        case ZYDIS_CATEGORY_CALL: return true;

        default: break;
    }

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
        case ZYDIS_CATEGORY_CALL: instr->features |= INSTR_CALL; break;
        case ZYDIS_CATEGORY_COND_BR: instr->features |= INSTR_JUMP; break;

        case ZYDIS_CATEGORY_UNCOND_BR:
            instr->features |= INSTR_JUMP | INSTR_STOP;
            break;

        default: break;
    };

    switch(zinstr.mnemonic) {
        case ZYDIS_MNEMONIC_HLT:
        case ZYDIS_MNEMONIC_INT3:
        case ZYDIS_MNEMONIC_RET: instr->features |= INSTR_STOP; break;
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
                op.type = OP_IMM;

                ZyanU64 addr = 0;

                if((instr->features & INSTR_JUMP ||
                    instr->features & INSTR_CALL) &&
                   ZYAN_SUCCESS(ZydisCalcAbsoluteAddress(
                       &zinstr, &zop, instr->address, &addr))) {
                    op.imm = addr;
                }
                else
                    op.imm = zop.imm.value.u;
                break;
            }

            case ZYDIS_OPERAND_TYPE_MEMORY: {
                if(zop.mem.base != ZYDIS_REGISTER_NONE &&
                   zop.mem.index != ZYDIS_REGISTER_NONE &&
                   !zop.mem.disp.has_displacement) {
                    op.type = OP_PHRASE;
                    op.phrase.base = zop.mem.base;
                    op.phrase.index = zop.mem.index;
                }
                else if(zop.mem.base == ZYDIS_REGISTER_NONE &&
                        zop.mem.index == ZYDIS_REGISTER_NONE) {
                    op.type = OP_MEM;

                    ZyanU64 addr;

                    if((instr->features & INSTR_JUMP ||
                        instr->features & INSTR_CALL) &&
                       ZYAN_SUCCESS(ZydisCalcAbsoluteAddress(
                           &zinstr, &zop, instr->address, &addr))) {
                        op.mem = addr;
                    }
                    op.mem = zop.mem.disp.value;
                }
                else {
                    op.type = OP_DISPL;
                    op.displ.base = zop.mem.base;
                    op.displ.index = zop.mem.index;
                    op.displ.scale = zop.mem.scale;
                    op.displ.displ = zop.mem.disp.value;
                }
                break;
            }

            default: break;
        }
    }
}

namespace {

bool render_instruction(const RDProcessor* /*self*/, const RDRendererParams* r,
                        const RDInstruction* instr) {
    const char* mnemonic =
        ZydisMnemonicGetString(static_cast<ZydisMnemonic>(instr->id));

    switch(instr->uservalue) {
        case ZYDIS_CATEGORY_COND_BR:
            rdrenderer_mnem(r->renderer, mnemonic, THEME_JUMPCOND);
            break;
        case ZYDIS_CATEGORY_UNCOND_BR:
            rdrenderer_mnem(r->renderer, mnemonic, THEME_JUMP);
            break;
        case ZYDIS_CATEGORY_CALL:
            rdrenderer_mnem(r->renderer, mnemonic, THEME_CALL);
            break;
        case ZYDIS_CATEGORY_RET:
            rdrenderer_mnem(r->renderer, mnemonic, THEME_RET);
            break;
        case ZYDIS_CATEGORY_NOP:
            rdrenderer_mnem(r->renderer, mnemonic, THEME_NOP);
            break;
        default: rdrenderer_mnem(r->renderer, mnemonic, THEME_DEFAULT); break;
    }

    foreach_operand(i, op, instr) {
        if(i > 0)
            rdrenderer_text(r->renderer, ", ");

        switch(op->type) {
            case OP_IMM: {
                if(is_addr_instruction(instr) && rd_isaddress(op->imm))
                    rdrenderer_addr(r->renderer, op->imm);
                else
                    rdrenderer_cnst(r->renderer, op->imm, 16);
                break;
            }

            case OP_MEM: {
                rdrenderer_text(r->renderer, "[");
                rdrenderer_addr(r->renderer, op->mem);
                rdrenderer_text(r->renderer, "]");
                break;
            }

            case OP_REG: {
                const char* reg =
                    ZydisRegisterGetString(static_cast<ZydisRegister>(op->reg));
                rdrenderer_reg(r->renderer, reg);
                break;
            }

            case OP_PHRASE: {
                rdrenderer_text(r->renderer, "[");

                const char* base = ZydisRegisterGetString(
                    static_cast<ZydisRegister>(op->phrase.base));
                rdrenderer_reg(r->renderer, base);

                rdrenderer_text(r->renderer, " + ");

                const char* index = ZydisRegisterGetString(
                    static_cast<ZydisRegister>(op->phrase.index));
                rdrenderer_reg(r->renderer, index);

                rdrenderer_text(r->renderer, "]");
                break;
            }

            case OP_DISPL: {
                rdrenderer_text(r->renderer, "[");

                const char* base = ZydisRegisterGetString(
                    static_cast<ZydisRegister>(op->displ.base));
                rdrenderer_reg(r->renderer, base);

                if(op->displ.index != ZYDIS_REGISTER_NONE) {
                    rdrenderer_text(r->renderer, " + ");

                    const char* index = ZydisRegisterGetString(
                        static_cast<ZydisRegister>(op->displ.index));
                    rdrenderer_reg(r->renderer, index);

                    if(op->displ.scale > 1) {
                        rdrenderer_text(r->renderer, " * ");
                        rdrenderer_cnst(r->renderer, op->displ.scale, 16);
                    }
                }

                if(op->displ.displ > 0) {
                    rdrenderer_text(r->renderer, " + ");
                    rdrenderer_addr(r->renderer, op->displ.displ);
                }

                rdrenderer_text(r->renderer, "]");
                break;
            }

            default: break;
        }
    }

    return true;
}

void decode(const RDProcessor* self, RDInstruction* instr) {
    reinterpret_cast<X86Processor*>(self->userdata)->decode(instr);
}

void emulate(const RDProcessor* /*self*/, RDEmulator* e,
             const RDInstruction* instr) {
    foreach_operand(i, op, instr) {
        switch(op->type) {
            case OP_IMM: {
                if(instr->features & INSTR_JUMP)
                    rdemulator_addref(e, op->imm, CR_JUMP);
                else if(instr->features & INSTR_CALL)
                    rdemulator_addref(e, op->imm, CR_CALL);
                else if(is_addr_instruction(instr) && rd_isaddress(op->imm))
                    rdemulator_addref(e, op->imm, DR_ADDRESS);
                else
                    rdemulator_addref(e, op->imm, DR_READ);

                break;
            }
            case OP_MEM: {
                if(instr->features & INSTR_JUMP) {
                    auto addr = x86_common::read_address(op->mem);
                    if(addr)
                        rdemulator_addref(e, *addr, CR_JUMP);
                }
                else if(instr->features & INSTR_CALL) {
                    auto addr = x86_common::read_address(op->mem);
                    if(addr)
                        rdemulator_addref(e, *addr, CR_CALL);
                }
                else if(!rd_istypenull(&op->dtype))
                    rd_settype(op->mem, &op->dtype);

                rdemulator_addref(e, op->mem, DR_READ);
                break;
            }

            default: break;
        }
    }

    if(!(instr->features & INSTR_STOP))
        rdemulator_addref(e, instr->address + instr->length, CR_FLOW);
}

RDProcessor x86_32{};
RDProcessor x86_64{};

} // namespace

void rdplugin_init() {
    x86_32.id = "x86_32";
    x86_32.name = "X86";
    x86_32.userdata = new X86Processor(32);
    x86_32.renderinstruction = render_instruction;
    x86_32.decode = decode;
    x86_32.emulate = emulate;
    x86_32.lift = x86_lifter::lift;
    rd_registerprocessor(&x86_32);

    x86_64.id = "x86_64";
    x86_64.name = "X64";
    x86_64.userdata = new X86Processor(64);
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
