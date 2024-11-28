#pragma once

#include <Zydis/Zydis.h>
#include <array>
#include <redasm/redasm.h>

class X86Processor {
    static constexpr usize BUFFER_SIZE = 256;

public:
    explicit X86Processor(usize bits);
    void decode(RDInstruction* instr);

private:
    ZydisDecoder m_decoder;
    std::array<char, ZYDIS_MAX_INSTRUCTION_LENGTH> m_dbuffer;
};
