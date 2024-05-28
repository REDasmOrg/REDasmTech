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
    void emulate(RDEmulateResult* r);

private:
    [[nodiscard]] ZydisRegister get_sp() const;
    [[nodiscard]] ZydisRegister get_bp() const;
    bool decode(RDAddress address);
    std::optional<RDAddress> calc_address(RDAddress address, usize idx,
                                          bool* istable = nullptr) const;

private:
    ZydisFormatter m_formatter;
    ZydisDecoder m_decoder;
    std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT> m_ops;
    std::array<char, ZYDIS_MAX_INSTRUCTION_LENGTH> m_dbuffer;
    std::array<char, BUFFER_SIZE> m_rbuffer;
    ZydisDecodedInstruction m_instr;
};
