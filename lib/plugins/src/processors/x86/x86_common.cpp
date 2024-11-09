#include "x86_common.h"

namespace x86_common {

std::optional<RDAddress> calc_address(const X86Instruction& instr,
                                      RDAddress address, usize idx) {
    if(idx >= instr.d.operand_count)
        return std::nullopt;

    RDAddress resaddr = 0;
    const ZydisDecodedOperand& zop = instr.ops[idx];

    switch(zop.type) {
        case ZYDIS_OPERAND_TYPE_IMMEDIATE: {
            if(!ZYAN_SUCCESS(ZydisCalcAbsoluteAddress(&instr.d, &zop, address,
                                                      &resaddr))) {
                return zop.imm.value.u;
            }

            return resaddr;
        }

        case ZYDIS_OPERAND_TYPE_MEMORY: {
            if(!ZYAN_SUCCESS(ZydisCalcAbsoluteAddress(&instr.d, &zop, address,
                                                      &resaddr))) {
                if((zop.mem.index != ZYDIS_REGISTER_NONE) &&
                   zop.mem.disp.has_displacement) {
                    // if(istable)
                    //     *istable = true;
                    return zop.mem.disp.value;
                }
            }
            else
                return resaddr;

            break;
        }

        default: break;
    }

    return std::nullopt;
}
ZydisRegister get_sp() {
    switch(rd_getbits()) {
        case 32: return ZYDIS_REGISTER_ESP;
        case 64: return ZYDIS_REGISTER_RSP;
        default: break;
    }

    return ZYDIS_REGISTER_SP;
}

ZydisRegister get_bp() {
    switch(rd_getbits()) {
        case 32: return ZYDIS_REGISTER_EBP;
        case 64: return ZYDIS_REGISTER_RBP;
        default: break;
    }

    return ZYDIS_REGISTER_BP;
}

} // namespace x86_common
