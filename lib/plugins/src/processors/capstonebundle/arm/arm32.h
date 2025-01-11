#pragma once

#include "../capstone.h"
#include <array>

struct ARM32Processor: public Capstone {
    explicit ARM32Processor(cs_mode mode);

    bool decode(RDInstruction* instr) override;
    void emulate(RDEmulator* e, const RDInstruction* instr) const override;
    void render_instruction(RDRenderer* r,
                            const RDInstruction* instr) const override;

private:
    void process_operands(RDInstruction* instr) const;
    [[nodiscard]] bool is_pc(int opidx) const;

private:
    std::array<u8, sizeof(u32)> m_buffer;
};

struct ARM32LEProcessor: public ARM32Processor {
    explicit ARM32LEProcessor(): ARM32Processor{CS_MODE_LITTLE_ENDIAN} {}
};

struct ARM32BEProcessor: public ARM32Processor {
    explicit ARM32BEProcessor(): ARM32Processor{CS_MODE_BIG_ENDIAN} {}
};
