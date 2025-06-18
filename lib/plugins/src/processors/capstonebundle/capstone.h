#pragma once

#include <capstone/capstone.h>
#include <redasm/redasm.h>

struct Capstone {
    Capstone(cs_arch a, cs_mode m);
    virtual ~Capstone();

    [[nodiscard]] const char* get_mnemonic(const RDInstruction* instr) const;
    [[nodiscard]] virtual const int* get_segmentregisters() const;
    [[nodiscard]] virtual const char* get_registername(int regid) const;
    [[nodiscard]] virtual RDAddress normalize_address(RDAddress address,
                                                      bool query) const;
    virtual void setup(RDEmulator* e) const { (void)e; }
    virtual bool decode(RDInstruction* instr) = 0;
    virtual void emulate(RDEmulator* e, const RDInstruction* instr) const = 0;
    virtual void render_instruction(RDRenderer* r,
                                    const RDInstruction* instr) const = 0;

protected:
    bool disasm(RDInstruction* instr, u8* data, usize size) const;

public:
    csh handle{};
    cs_insn* insn{nullptr};
    cs_arch arch;
    cs_mode mode;
};
