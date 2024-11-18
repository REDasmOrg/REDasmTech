#include "x86_common.h"

namespace x86_common {

namespace {

std::optional<RDAddress> read_address(RDAddress address) {
    RDValue val;

    if(rd_getbits() == 64) {
        if(rd_gettype(address, "u64", &val))
            return val.u64_v;

        return std::nullopt;
    }

    if(rd_gettype(address, "u32", &val))
        return val.u32_v;

    return std::nullopt;
}

} // namespace

std::optional<RDAddress> calc_address(const X86Instruction& instr, usize idx,
                                      std::optional<RDAddress>& dstaddr) {
    dstaddr = std::nullopt;

    if(idx >= instr.d.operand_count)
        return std::nullopt;

    RDAddress resaddr = 0;
    const ZydisDecodedOperand& zop = instr.ops[idx];

    switch(zop.type) {
        case ZYDIS_OPERAND_TYPE_IMMEDIATE: {
            if(!ZYAN_SUCCESS(ZydisCalcAbsoluteAddress(
                   &instr.d, &zop, instr.address, &resaddr))) {
                return zop.imm.value.u;
            }

            return resaddr;
        }

        case ZYDIS_OPERAND_TYPE_MEMORY: {
            if(!ZYAN_SUCCESS(ZydisCalcAbsoluteAddress(
                   &instr.d, &zop, instr.address, &resaddr))) {
                if((zop.mem.index != ZYDIS_REGISTER_NONE) &&
                   zop.mem.disp.has_displacement) {
                    // if(istable)
                    //     *istable = true;

                    dstaddr = x86_common::read_address(zop.mem.disp.value);
                    return zop.mem.disp.value;
                }
            }
            else {
                dstaddr = x86_common::read_address(resaddr);
                return resaddr;
            }

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
