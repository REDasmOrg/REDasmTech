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

usize X86Processor::emulate(RDEmulateResult* r) {
    if(!this->decode(r->address))
        return 0;

    switch(m_instr.meta.category) {
        case ZYDIS_CATEGORY_CALL: {
            bool istable = false;
            auto addr = this->calc_address(r->address, 0, &istable);

            if(addr) {
                if(istable) {
                }
                else {
                    rd_setfunction(*addr);
                    rd_schedule(*addr);
                }
            }
            break;
        }

        case ZYDIS_CATEGORY_UNCOND_BR: {
            r->canflow = false;
            bool istable = false;
            auto addr = this->calc_address(r->address, 0, &istable);

            if(addr) {
                if(istable) {
                }
                else {
                    rd_setbranch(*addr);
                    rd_schedule(*addr);
                }
            }

            break;
        }

        case ZYDIS_CATEGORY_COND_BR: {
            auto addr = this->calc_address(r->address, 0);
            if(addr) {
                rd_setbranchtrue(*addr);
                rd_schedule(*addr);
            }

            rd_setbranchfalse(r->address + m_instr.length);
            break;
        }

        case ZYDIS_CATEGORY_SYSTEM: {
            if(m_instr.mnemonic == ZYDIS_MNEMONIC_HLT)
                r->canflow = false;
            break;
        }

        case ZYDIS_CATEGORY_INTERRUPT: {
            if(m_instr.mnemonic == ZYDIS_MNEMONIC_INT3)
                r->canflow = false;
            break;
        }

        case ZYDIS_CATEGORY_RET: r->canflow = false; break;
        default: break;
    }

    return m_instr.length;
}

std::optional<RDAddress>
X86Processor::calc_address(RDAddress address, usize idx, bool* istable) const {
    if(idx >= m_instr.operand_count)
        return std::nullopt;

    RDAddress resaddr = 0;
    const ZydisDecodedOperand_& zop = m_ops[idx];

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
                    if(istable)
                        *istable = true;
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

usize emulate(const RDProcessor* self, RDEmulateResult* r) {
    return reinterpret_cast<X86Processor*>(self->userdata)->emulate(r);
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
