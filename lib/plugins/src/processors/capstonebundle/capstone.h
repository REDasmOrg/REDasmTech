#pragma once

#include <capstone/capstone.h>
#include <redasm/redasm.h>

struct Capstone {
    Capstone(cs_arch arch, cs_mode mode);
    virtual ~Capstone();

    [[nodiscard]] const char* get_registername(int regid) const;
    virtual bool decode(RDInstruction* instr) = 0;
    virtual void emulate(RDEmulator* e, const RDInstruction* instr) const = 0;
    virtual void render_instruction(RDRenderer* r,
                                    const RDInstruction* instr) const = 0;

protected:
    bool disasm(RDInstruction* instr, u8* data, usize size) const;

public:
    csh handle{};
    cs_insn* insn{nullptr};
};
