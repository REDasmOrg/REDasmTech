#include "dalvik_types.h"
#include <string>

namespace dalvik {

namespace {

std::string gen_name(RDAddress address, const std::string& prefix) {
    return prefix + "_" + rd_tohex(address);
}

} // namespace

void create_packedswitchpayload(RDAddress address,
                                const PackedSwitchPayload* payload) {
    rd_settypename(address, "PACKED_SWITCH_PAYLOAD", nullptr);

    RDType t;
    if(!rdtype_create_n("i32", payload->size, &t)) return;

    RDAddress tgtaddress = address + rd_sizeof("PACKED_SWITCH_PAYLOAD");
    if(rd_settype(tgtaddress, &t, nullptr))
        rd_setname(tgtaddress, dalvik::gen_name(tgtaddress, "targets").c_str());
}

} // namespace dalvik
