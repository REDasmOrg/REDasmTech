#include "x86_common.h"

namespace x86_common {

std::optional<RDAddress> read_address(RDAddress address) {
    const RDProcessorPlugin* p = rd_getprocessorplugin();
    bool ok = false;
    RDAddress val;

    if(p->address_size == 8) {
        u64 v;
        ok = rd_getu64(address, &v);
        val = v;
    }
    else if(p->address_size == 4) {
        u32 v;
        ok = rd_getu32(address, &v);
        val = v;
    }

    return ok ? std::make_optional(val) : std::nullopt;
}

ZydisRegister get_sp() {
    switch(rd_getprocessorplugin()->address_size) {
        case 4: return ZYDIS_REGISTER_ESP;
        case 8: return ZYDIS_REGISTER_RSP;
        default: break;
    }

    return ZYDIS_REGISTER_SP;
}

ZydisRegister get_bp() {
    switch(rd_getprocessorplugin()->address_size) {
        case 4: return ZYDIS_REGISTER_EBP;
        case 8: return ZYDIS_REGISTER_RBP;
        default: break;
    }

    return ZYDIS_REGISTER_BP;
}

} // namespace x86_common
