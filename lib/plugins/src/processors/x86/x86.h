#pragma once

#include "x86_common.h"
#include <Zydis/Zydis.h>
#include <array>
#include <redasm/redasm.h>

class X86Processor {
    static constexpr usize BUFFER_SIZE = 256;

public:
    explicit X86Processor(usize bits);
    bool render_instruction(const RDRendererParams* r);
    bool lift(RDAddress address, RDILList* l);
    void emulate(RDEmulator* e, RDInstructionDetail* detail);

private:
    void process_refs(RDAddress address, RDEmulator* e) const;
    void process_imm_op(const ZydisDecodedOperand& op, RDEmulator* e) const;
    void process_mem_op(const ZydisDecodedOperand& op, RDEmulator* e) const;
    void set_type(RDAddress address, const ZydisDecodedOperand& op,
                  RDEmulator* e) const;
    bool decode(RDAddress address);

private:
    ZydisFormatter m_formatter;
    ZydisDecoder m_decoder;
    X86Instruction m_instr;
    std::array<char, ZYDIS_MAX_INSTRUCTION_LENGTH> m_dbuffer;
    std::array<char, BUFFER_SIZE> m_rbuffer;
};
