#pragma once

#include "../capstone.h"
#include <array>

namespace arm32 {

constexpr int ARM_REG_T = ARM_REG_ENDING + 1;

struct ARMCommon: public Capstone {
    explicit ARMCommon(cs_mode mode);

    [[nodiscard]] virtual u64 t_reg() const = 0;
    [[nodiscard]] const char* get_registername(int reg) const override;
    [[nodiscard]] RDAddress normalize_address(RDAddress address) const override;
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

namespace impl {

struct ARMLEImpl: public ARMCommon {
    explicit ARMLEImpl(): ARMCommon{CS_MODE_LITTLE_ENDIAN} {}
    [[nodiscard]] u64 t_reg() const override { return 0; }
};

struct ARMBEImpl: public ARMCommon {
    explicit ARMBEImpl(): ARMCommon{CS_MODE_BIG_ENDIAN} {}
    [[nodiscard]] u64 t_reg() const override { return 0; }
};

struct THUMBLEImpl: public ARMCommon {
    explicit THUMBLEImpl()
        : ARMCommon{
              static_cast<cs_mode>(CS_MODE_THUMB | CS_MODE_LITTLE_ENDIAN)} {}
    [[nodiscard]] u64 t_reg() const override { return 1; }
};

struct THUMBBEImpl: public ARMCommon {
    explicit THUMBBEImpl()
        : ARMCommon{static_cast<cs_mode>(CS_MODE_THUMB | CS_MODE_BIG_ENDIAN)} {}
    [[nodiscard]] u64 t_reg() const override { return 1; }
};

template<typename ARM, typename THUMB>
struct ARMImpl {
    bool decode(RDInstruction* instr) {
        auto reg = rdemulator_getreg(rd_getemulator(), ARM_REG_T);
        if(RDRegValue_value_or(reg, 0) == 0) return m_arm.decode(instr);
        return m_thumb.decode(instr);
    }

    [[nodiscard]] const char* get_registername(int reg) const {
        return m_arm.get_registername(reg);
    }

    [[nodiscard]] RDAddress normalize_address(RDAddress address) const {
        return m_arm.normalize_address(address);
    }

    void emulate(RDEmulator* e, const RDInstruction* instr) const {
        auto reg = rdemulator_getreg(rd_getemulator(), ARM_REG_T);
        if(RDRegValue_value_or(reg, 0) == 0) return m_arm.emulate(e, instr);
        return m_thumb.emulate(e, instr);
    }

    bool render_instruction(RDRenderer* r, const RDInstruction* instr) const {
        // if(rd_getsreg_ex(instr->address, ARM_REG_T, 0) == 0)
        //     return m_arm.render_instruction(r, instr);
        // return m_thumb.render_instruction(r, instr);

        return false;
    }

    [[nodiscard]] const char* get_mnemonic(const RDInstruction* instr) const {
        // if(rd_getsreg_ex(instr->address, ARM_REG_T, 0) == 0)
        // return m_arm.get_mnemonic(instr);
        // return m_thumb.get_mnemonic(instr);

        return nullptr;
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
