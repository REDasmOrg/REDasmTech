#include "x86_common.h"

namespace x86_common {

std::optional<RDAddress> read_address(RDAddress address) {
    if(rd_getbits() == 64) {
        u64 val;

        if(rd_getu64(address, &val))
            return val;

        return std::nullopt;
    }

    if(rd_getbits() == 32) {
        u32 val;

        if(rd_getu32(address, &val))
            return val;
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
