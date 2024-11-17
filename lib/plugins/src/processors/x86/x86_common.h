#pragma once

#include <Zydis/Zydis.h>
#include <array>
#include <optional>
#include <redasm/redasm.h>

using X86Operands = std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT>;

struct X86Instruction {
    RDAddress address;
    ZydisDecodedInstruction d;
    X86Operands ops;
};

namespace x86_common {

[[nodiscard]] std::optional<RDAddress>
calc_address(const X86Instruction& instr, usize idx,
             std::optional<RDAddress>& memaddr);

[[nodiscard]] inline std::optional<RDAddress>
calc_address(const X86Instruction& instr, usize idx) {
    std::optional<RDAddress> memaddr;
    return x86_common::calc_address(instr, idx, memaddr);
}

ZydisRegister get_sp();
ZydisRegister get_bp();

} // namespace x86_common
