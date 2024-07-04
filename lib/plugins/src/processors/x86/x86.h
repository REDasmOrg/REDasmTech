#pragma once

#include <Zydis/Zydis.h>
#include <array>
#include <optional>
#include <redasm/redasm.h>

class X86Processor {
    static constexpr usize BUFFER_SIZE = 256;

public:
    explicit X86Processor(usize bits);
    bool render_instruction(const RDRendererParams* r);
    bool lift(RDAddress address, RDILList* l);
    usize emulate(RDAddress address, RDEmulator* e);

private:
    [[nodiscard]] ZydisRegister get_sp() const;
    [[nodiscard]] ZydisRegister get_bp() const;
    void process_refs(RDAddress address, RDEmulator* e) const;
    void process_imm_op(const ZydisDecodedOperand& op, RDEmulator* e) const;
    void process_mem_op(const ZydisDecodedOperand& op, RDEmulator* e) const;
    void set_type(RDAddress address, const ZydisDecodedOperand& op,
                  RDEmulator* e) const;
    bool decode(RDAddress address);
    [[nodiscard]] std::optional<RDAddress> calc_address(RDAddress address,
                                                        usize idx) const;

private:
    ZydisFormatter m_formatter;
    ZydisDecoder m_decoder;
    std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT> m_ops;
    std::array<char, ZYDIS_MAX_INSTRUCTION_LENGTH> m_dbuffer;
    std::array<char, BUFFER_SIZE> m_rbuffer;
    ZydisDecodedInstruction m_instr;
};
