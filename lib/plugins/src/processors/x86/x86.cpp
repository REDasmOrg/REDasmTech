#include "x86.h"
#include <redasm/redasm.h>
#include <string>

X86Processor::X86Processor(usize bits) {
    if(bits == 32) {
        ZydisDecoderInit(&m_decoder, ZYDIS_MACHINE_MODE_LEGACY_32,
                         ZYDIS_STACK_WIDTH_32);
    }
    else {
        ZydisDecoderInit(&m_decoder, ZYDIS_MACHINE_MODE_LONG_64,
                         ZYDIS_STACK_WIDTH_64);
    }

    ZydisFormatterInit(&m_formatter, ZYDIS_FORMATTER_STYLE_INTEL);
    ZydisFormatterSetProperty(&m_formatter, ZYDIS_FORMATTER_PROP_HEX_PREFIX,
                              ZYAN_FALSE);
}

ZydisRegister X86Processor::get_sp() const {
    switch(rd_getbits()) {
        case 32: return ZYDIS_REGISTER_ESP;
        case 64: return ZYDIS_REGISTER_RSP;
        default: break;
    }

    return ZYDIS_REGISTER_SP;
}

ZydisRegister X86Processor::get_bp() const {
    switch(rd_getbits()) {
        case 32: return ZYDIS_REGISTER_EBP;
        case 64: return ZYDIS_REGISTER_RBP;
        default: break;
    }

    return ZYDIS_REGISTER_BP;
}

bool X86Processor::decode(RDAddress address) {
    usize n = rd_memoryread(address, m_dbuffer.data(), m_dbuffer.size());
    if(!n)
        return false;

    ZyanStatus s = ZydisDecoderDecodeFull(&m_decoder, m_dbuffer.data(), n,
                                          &m_instr, m_ops.data());
    return ZYAN_SUCCESS(s);
}

bool X86Processor::render_instruction(const RDRendererParams* r) {
    if(!this->decode(r->address))
        return false;

    ZydisFormatterTokenConst* token = nullptr;
    ZyanStatus s = ZydisFormatterTokenizeInstruction(
        &m_formatter, &m_instr, m_ops.data(), m_instr.operand_count,
        m_rbuffer.data(), m_rbuffer.size(), r->address, &token, nullptr);

    if(!ZYAN_SUCCESS(s))
        return false;

    ZydisTokenType tokentype;
    ZyanConstCharPointer tokenvalue = nullptr;

    while(token) {
        ZydisFormatterTokenGetValue(token, &tokentype, &tokenvalue);

        switch(tokentype) {
            case ZYDIS_TOKEN_ADDRESS_ABS:
            case ZYDIS_TOKEN_ADDRESS_REL:
            case ZYDIS_TOKEN_IMMEDIATE:
            case ZYDIS_TOKEN_DISPLACEMENT: {
                usize v = std::stoull(tokenvalue, nullptr, 16);
                rdrenderer_reference(r->renderer, v);
                break;
            }

            case ZYDIS_TOKEN_MNEMONIC: {
                if(m_instr.meta.category == ZYDIS_CATEGORY_COND_BR)
                    rdrenderer_themed(r->renderer, tokenvalue, THEME_JUMPCOND);
                else if(m_instr.meta.category == ZYDIS_CATEGORY_UNCOND_BR)
                    rdrenderer_themed(r->renderer, tokenvalue, THEME_JUMP);
                else if(m_instr.meta.category == ZYDIS_CATEGORY_CALL)
                    rdrenderer_themed(r->renderer, tokenvalue, THEME_CALL);
                else if(m_instr.meta.category == ZYDIS_CATEGORY_RET)
                    rdrenderer_themed(r->renderer, tokenvalue, THEME_RET);
                else if(m_instr.meta.category == ZYDIS_CATEGORY_NOP)
                    rdrenderer_themed(r->renderer, tokenvalue, THEME_NOP);
                else
                    rdrenderer_themed(r->renderer, tokenvalue, THEME_DEFAULT);
                break;
            }

            case ZYDIS_TOKEN_REGISTER:
                rdrenderer_register(r->renderer, tokenvalue);
                break;

            default: rdrenderer_text(r->renderer, tokenvalue); break;
        }

        if(!ZYAN_SUCCESS(ZydisFormatterTokenNext(&token)))
            token = nullptr;
    }

    return true;
}

usize X86Processor::emulate(RDAddress address, RDEmulator* e) {
    if(!this->decode(address))
        return 0;

    bool flow = true;

    switch(m_instr.meta.category) {
        case ZYDIS_CATEGORY_CALL: {
            auto addr = this->calc_address(address, 0);

            if(addr)
                rdemulator_addcoderef(e, *addr, CR_CALL);
            break;
        }

        case ZYDIS_CATEGORY_UNCOND_BR: {
            flow = false;
            auto addr = this->calc_address(address, 0);

            if(addr)
                rdemulator_addcoderef(e, *addr, CR_JUMP);

            break;
        }

        case ZYDIS_CATEGORY_COND_BR: {
            auto addr = this->calc_address(address, 0);
            if(addr)
                rdemulator_addcoderef(e, *addr, CR_JUMP);
            break;
        }

        case ZYDIS_CATEGORY_SYSTEM: {
            if(m_instr.mnemonic == ZYDIS_MNEMONIC_HLT)
                flow = false;
            break;
        }

        case ZYDIS_CATEGORY_INTERRUPT: {
            if(m_instr.mnemonic == ZYDIS_MNEMONIC_INT3)
                flow = false;
            break;
        }

        case ZYDIS_CATEGORY_RET: flow = false; break;
        default: this->process_refs(address, e); break;
    }

    if(flow)
        rdemulator_addcoderef(e, address + m_instr.length, CR_FLOW);

    return m_instr.length;
}

void X86Processor::process_refs(RDAddress address, RDEmulator* e) const {
    if(m_instr.mnemonic == ZYDIS_MNEMONIC_LEA) {
        return;
    }

    if(address == 0x0040100e) {
        int zzz = 0;
        zzz++;
    }

    for(auto i = 0; i < m_instr.operand_count; i++) {
        const ZydisDecodedOperand& op = m_ops[i];
        if(op.element_count != 1)
            continue;

        switch(op.type) {
            case ZYDIS_OPERAND_TYPE_MEMORY: this->process_mem_op(op, e); break;

            case ZYDIS_OPERAND_TYPE_IMMEDIATE:
                this->process_imm_op(op, e);
                break;

            case ZYDIS_OPERAND_TYPE_REGISTER: break;

            default: break;
        }
    }
}

void X86Processor::process_imm_op(const ZydisDecodedOperand& op,
                                  RDEmulator* e) const {
    usize r = op.actions == ZYDIS_OPERAND_ACTION_WRITE ? DR_WRITE : DR_READ;

    switch(op.encoding) {
        case ZYDIS_OPERAND_ENCODING_SIMM16_32_32: {
            this->set_type(op.imm.value.s, op);
            rdemulator_adddataref(e, op.imm.value.s, r);
            break;
        }

        default: break;
    }
}

void X86Processor::process_mem_op(const ZydisDecodedOperand& op,
                                  RDEmulator* e) const {

    if(op.element_count > 1)
        return;

    usize r = op.actions == ZYDIS_OPERAND_ACTION_WRITE ? DR_WRITE : DR_READ;

    switch(op.encoding) {
        case ZYDIS_OPERAND_ENCODING_DISP16_32_64: {
            this->set_type(op.mem.disp.value, op);
            rdemulator_adddataref(e, op.mem.disp.value, r);
            break;
        }

        case ZYDIS_OPERAND_ENCODING_MODRM_RM: {
            if(op.mem.segment == ZYDIS_REGISTER_DS &&
               op.mem.disp.has_displacement &&
               rd_isaddress(op.mem.disp.value)) {

                this->set_type(op.mem.disp.value, op);
                rdemulator_adddataref(e, op.mem.disp.value, r);
            }

            break;
        }

        default: break;
    }
}

void X86Processor::set_type(RDAddress address,
                            const ZydisDecodedOperand& op) const {

    switch(op.element_type) {
        case ZYDIS_ELEMENT_TYPE_INT: {
            std::string tname = "i" + std::to_string(op.element_size);
            rd_settype(address, tname.c_str());
            break;
        }

        case ZYDIS_ELEMENT_TYPE_UINT: {
            std::string tname = "u" + std::to_string(op.element_size);
            rd_settype(address, tname.c_str());
            break;
        }

        default: break;
    }
}

std::optional<RDAddress> X86Processor::calc_address(RDAddress address,
                                                    usize idx) const {
    if(idx >= m_instr.operand_count)
        return std::nullopt;

    RDAddress resaddr = 0;
    const ZydisDecodedOperand& zop = m_ops[idx];

    switch(zop.type) {
        case ZYDIS_OPERAND_TYPE_IMMEDIATE: {
            if(!ZYAN_SUCCESS(ZydisCalcAbsoluteAddress(&m_instr, &zop, address,
                                                      &resaddr))) {
                return zop.imm.value.u;
            }

            return resaddr;
        }

        case ZYDIS_OPERAND_TYPE_MEMORY: {
            if(!ZYAN_SUCCESS(ZydisCalcAbsoluteAddress(&m_instr, &zop, address,
                                                      &resaddr))) {
                if((zop.mem.index != ZYDIS_REGISTER_NONE) &&
                   zop.mem.disp.has_displacement) {
                    // if(istable)
                    //     *istable = true;
                    return zop.mem.disp.value;
                }
            }
            else
                return resaddr;

            break;
        }

        default: break;
    }

    return std::nullopt;
}

namespace {

bool render_instruction(const RDProcessor* self, const RDRendererParams* r) {
    return reinterpret_cast<X86Processor*>(self->userdata)
        ->render_instruction(r);
}

usize emulate(const RDProcessor* self, RDAddress address, RDEmulator* e) {
    return reinterpret_cast<X86Processor*>(self->userdata)->emulate(address, e);
}

RDProcessor x86_32{};
RDProcessor x86_64{};

} // namespace

void rdplugin_init() {
    x86_32.name = "x86_32";
    x86_32.userdata = new X86Processor(32);
    x86_32.renderinstruction = render_instruction;
    x86_32.emulate = emulate;
    rd_registerprocessor(&x86_32);

    x86_64.name = "x86_64";
    x86_64.userdata = new X86Processor(64);
    x86_64.renderinstruction = render_instruction;
    x86_64.emulate = emulate;
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
