#include "x86_common.h"

namespace x86_common {

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
