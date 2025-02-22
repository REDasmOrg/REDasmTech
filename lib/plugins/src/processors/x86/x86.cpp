#include "x86_common.h"
#include "x86_lifter.h"
#include <Zydis/Zydis.h>
#include <array>
#include <iostream>
#include <redasm/redasm.h>

namespace {

struct X86Processor {
    ZydisDecoder decoder;
    std::array<char, ZYDIS_MAX_INSTRUCTION_LENGTH> buffer;
};

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

    if(op.dtype.id && zop.element_count > 1) op.dtype.n = zop.element_count;
}

bool is_segment_reg(const RDOperand& op) {
    if(op.type != OP_REG) return false;

    switch(op.reg) {
        case ZYDIS_REGISTER_ES:
        case ZYDIS_REGISTER_CS:
        case ZYDIS_REGISTER_SS:
        case ZYDIS_REGISTER_DS:
        case ZYDIS_REGISTER_FS:
        case ZYDIS_REGISTER_GS: return true;

        default: break;
    }

    return false;
}

bool is_branch_instruction(const RDInstruction* instr) {
    return (instr->features & (IF_JUMP | IF_CALL));
}

bool maybe_addr(const RDInstruction* instr) {
    switch(instr->id) {
        case ZYDIS_MNEMONIC_PUSH: break;
        default: return false;
    }

    return true;
}

void track_segment_reg(RDEmulator* e, const RDInstruction* instr) {
    RDInstruction previnstr;
    if(!rd_decode_prev(instr->address, &previnstr)) return;

    if(previnstr.id != ZYDIS_MNEMONIC_PUSH ||
       !is_segment_reg(previnstr.operands[0]))
        return;

    int chgreg = instr->operands[0].reg;

    if(previnstr.operands[0].reg == ZYDIS_REGISTER_CS) {
        const RDSegment* seg = rdemulator_getsegment(e);
        if(seg) {
            rdemulator_addregchange(e, instr->address + instr->length, chgreg,
                                    seg->start);
        }
    }
    else {
        u64 val = rdemulator_getreg(e, previnstr.operands[0].reg);
        rdemulator_addregchange(e, instr->address + instr->length, chgreg, val);
    }
}

void decode(RDProcessor* proc, RDInstruction* instr) {
    auto* self = reinterpret_cast<X86Processor*>(proc);
    usize n = rd_read(instr->address, self->buffer.data(), self->buffer.size());

    ZydisDecodedInstruction zinstr;
    std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT> zops;

    if(!n ||
       !ZYAN_SUCCESS(ZydisDecoderDecodeFull(&self->decoder, self->buffer.data(),
                                            n, &zinstr, zops.data()))) {
        return;
    }

    instr->id = zinstr.mnemonic;
    instr->length = zinstr.length;

    switch(zinstr.meta.category) {
        case ZYDIS_CATEGORY_CALL: instr->features |= IF_CALL; break;
        case ZYDIS_CATEGORY_COND_BR: instr->features |= IF_JUMP; break;
        case ZYDIS_CATEGORY_NOP: instr->features |= IF_NOP; break;

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
        if(zop.visibility == ZYDIS_OPERAND_VISIBILITY_HIDDEN) continue;

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
                else if(maybe_addr(instr) && rd_isaddress(zop.imm.value.u)) {
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

void render_instruction(const RDProcessor* /*self*/, RDRenderer* r,
                        const RDInstruction* instr) {
    if(instr->features & IF_STOP) {
        if(instr->features & IF_JUMP)
            rdrenderer_mnem(r, instr->id, THEME_JUMP);
        else
            rdrenderer_mnem(r, instr->id, THEME_RET);
    }
    else if(instr->features & IF_JUMP)
        rdrenderer_mnem(r, instr->id, THEME_JUMPCOND);
    else if(instr->features & IF_CALL)
        rdrenderer_mnem(r, instr->id, THEME_CALL);
    else if(instr->features & IF_NOP)
        rdrenderer_mnem(r, instr->id, THEME_NOP);
    else
        rdrenderer_mnem(r, instr->id, THEME_DEFAULT);

    foreach_operand(i, op, instr) {
        if(i > 0) rdrenderer_text(r, ", ");

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

void emulate(RDProcessor* /*self*/, RDEmulator* e, const RDInstruction* instr) {
    if(instr->id == ZYDIS_MNEMONIC_POP && is_segment_reg(instr->operands[0])) {
        track_segment_reg(e, instr);
    }
    else {
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
                        if(addr) rdemulator_addref(e, *addr, CR_JUMP);
                    }
                    else if(instr->features & IF_CALL) {
                        auto addr = x86_common::read_address(op->mem);
                        if(addr) rdemulator_addref(e, *addr, CR_CALL);
                    }

                    rdemulator_addref(e, op->mem, DR_READ);
                    break;
                }

                case OP_REG: {
                    if(!i && op->reg == ZYDIS_REGISTER_DS) {
                        std::cout << "DS CHANGE IN " << std::hex
                                  << instr->address << '\n';
                    }
                    else if(!i && op->reg == ZYDIS_REGISTER_CS) {
                        std::cout << "CS CHANGE IN " << std::hex
                                  << instr->address << '\n';
                    }
                    break;
                }

                default: break;
            }
        }
    }

    if(!(instr->features & IF_STOP))
        rdemulator_flow(e, instr->address + instr->length);
}

const char* get_mnemonic(const RDProcessor*, u32 id) {
    return ZydisMnemonicGetString(static_cast<ZydisMnemonic>(id));
}

const char* get_register_name(const RDProcessor*, int reg) {
    return ZydisRegisterGetString(static_cast<ZydisRegister>(reg));
}

template<ZydisMachineMode Mode, ZydisStackWidth Width>
void register_processor(RDProcessorPlugin* plugin, const char* id,
                        const char* name, int addrsize, int intsize) {
    plugin->level = REDASM_API_LEVEL;
    plugin->id = id;
    plugin->name = name;
    plugin->address_size = addrsize;
    plugin->integer_size = intsize;
    plugin->get_mnemonic = get_mnemonic;
    plugin->get_registername = get_register_name;
    plugin->decode = decode;
    plugin->emulate = emulate;
    plugin->lift = x86_lifter::lift;
    plugin->render_instruction = render_instruction;

    plugin->create = [](const RDProcessorPlugin*) {
        auto* self = new X86Processor();
        ZydisDecoderInit(&self->decoder, Mode, Width);
        return reinterpret_cast<RDProcessor*>(self);
    };

    plugin->destroy = [](RDProcessor* self) {
        delete reinterpret_cast<X86Processor*>(self);
    };

    rd_registerprocessor(plugin);
}

RDProcessorPlugin x86_16_real;
RDProcessorPlugin x86_16;
RDProcessorPlugin x86_32;
RDProcessorPlugin x86_64;

} // namespace

void rdplugin_create() {
    // clang-format off
    register_processor<ZYDIS_MACHINE_MODE_REAL_16, ZYDIS_STACK_WIDTH_16>(&x86_16_real, "x86_16_real", "X86_16 (Real Mode)", 2, 2);
    register_processor<ZYDIS_MACHINE_MODE_LEGACY_16, ZYDIS_STACK_WIDTH_16>(&x86_16, "x86_16", "X86_16", 2, 2);
    register_processor<ZYDIS_MACHINE_MODE_LEGACY_32, ZYDIS_STACK_WIDTH_32>(&x86_32, "x86_32", "X86_32", 4, 4);
    register_processor<ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64>(&x86_64, "x86_64", "X86_64", 8, 4);
    // clang-format on
}
