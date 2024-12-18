#include "x86_common.h"

namespace x86_common {

std::optional<RDAddress> read_address(RDAddress address) {
    const RDProcessor* p = rd_getprocessor();

    if(p->address_size == 8) {
        u64 val;

        if(rd_getu64(address, &val))
            return val;

        return std::nullopt;
    }

    if(p->address_size == 4) {
        u32 val;

        if(rd_getu32(address, &val))
            return val;
    }

    return std::nullopt;
}

ZydisRegister get_sp() {
    switch(rd_getprocessor()->address_size) {
        case 4: return ZYDIS_REGISTER_ESP;
        case 8: return ZYDIS_REGISTER_RSP;
        default: break;
    }

    return ZYDIS_REGISTER_SP;
}

ZydisRegister get_bp() {
    switch(rd_getprocessor()->address_size) {
        case 4: return ZYDIS_REGISTER_EBP;
        case 8: return ZYDIS_REGISTER_RBP;
        default: break;
    }

    return ZYDIS_REGISTER_BP;
}

} // namespace x86_common
