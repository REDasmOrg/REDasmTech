#pragma once

#include "../capstone.h"
#include <array>

namespace arm32 {

constexpr int ARM_REG_T = ARM_REG_ENDING + 1;

struct ARMCommon: public Capstone {
    explicit ARMCommon(cs_mode mode);
    [[nodiscard]] const int* get_segmentregisters() const override;
    [[nodiscard]] const char* get_registername(int reg) const override;
    [[nodiscard]] RDAddress normalize_address(RDAddress address) const override;
    [[nodiscard]] RDAddress pc(const RDInstruction* instr) const;
    bool decode(RDInstruction* instr) override;
    void emulate(RDEmulator* e, const RDInstruction* instr) const override;
    void render_instruction(RDRenderer* r,
                            const RDInstruction* instr) const override;

private:
    bool check_fixups(RDInstruction* instr, const cs_arm& arm) const;
    void process_operand(RDInstruction* instr, usize idx,
                         const cs_arm_op& op) const;
    void process_operands(RDInstruction* instr) const;
    [[nodiscard]] bool is_pc(int opidx) const;

private:
    std::array<u8, sizeof(u32)> m_buffer;
};

namespace impl {

struct ARMLEImpl: public ARMCommon {
    explicit ARMLEImpl(): ARMCommon{CS_MODE_LITTLE_ENDIAN} {}
};

struct ARMBEImpl: public ARMCommon {
    explicit ARMBEImpl(): ARMCommon{CS_MODE_BIG_ENDIAN} {}
};

struct THUMBLEImpl: public ARMCommon {
    explicit THUMBLEImpl()
        : ARMCommon{
              static_cast<cs_mode>(CS_MODE_THUMB | CS_MODE_LITTLE_ENDIAN)} {}
};

struct THUMBBEImpl: public ARMCommon {
    explicit THUMBBEImpl()
        : ARMCommon{static_cast<cs_mode>(CS_MODE_THUMB | CS_MODE_BIG_ENDIAN)} {}
};

template<typename ARM, typename THUMB>
struct ARMImpl {
    bool decode(RDInstruction* instr) {
        auto reg = rd_getsreg(instr->address, ARM_REG_T);
        if(RDRegValue_value_or(reg, 0) == 0) return m_arm.decode(instr);
        return m_thumb.decode(instr);
    }

    void setup(RDEmulator* e) const { m_arm.setup(e); }

    [[nodiscard]] const int* get_segmentregisters() const {
        return m_arm.get_segmentregisters();
    }

    [[nodiscard]] const char* get_registername(int reg) const {
        return m_arm.get_registername(reg);
    }

    [[nodiscard]] RDAddress normalize_address(RDAddress address) const {
        return m_arm.normalize_address(address);
    }

    void emulate(RDEmulator* e, const RDInstruction* instr) const {
        auto reg = rd_getsreg(instr->address, ARM_REG_T);
        if(RDRegValue_value_or(reg, 0) == 0) return m_arm.emulate(e, instr);
        return m_thumb.emulate(e, instr);
    }

    void render_instruction(RDRenderer* r, const RDInstruction* instr) const {
        auto reg = rd_getsreg(instr->address, ARM_REG_T);
        if(RDRegValue_value_or(reg, 0) == 0)
            m_arm.render_instruction(r, instr);
        else
            m_thumb.render_instruction(r, instr);
    }

    [[nodiscard]] const char* get_mnemonic(const RDInstruction* instr) const {
        auto reg = rd_getsreg(instr->address, ARM_REG_T);
        if(RDRegValue_value_or(reg, 0) == 0) return m_arm.get_mnemonic(instr);
        return m_thumb.get_mnemonic(instr);
    }

private:
    ARM m_arm;
    THUMB m_thumb;
};

} // namespace impl

using ARMLE = impl::ARMImpl<impl::ARMLEImpl, impl::THUMBLEImpl>;
using ARMBE = impl::ARMImpl<impl::ARMBEImpl, impl::THUMBBEImpl>;
using THUMBLE = impl::ARMImpl<impl::THUMBLEImpl, impl::ARMLEImpl>;
using THUMBBE = impl::ARMImpl<impl::THUMBBEImpl, impl::ARMBEImpl>;

} // namespace arm32
