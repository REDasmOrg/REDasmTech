#include "dalvik_types.h"
#include <string>

namespace dalvik {

namespace {

std::string gen_name(RDAddress address, const std::string& prefix) {
    return prefix + "_" + rd_tohex(address);
}

} // namespace

const i32* create_packedswitchpayload(RDAddress address,
                                      const PackedSwitchPayload* payload) {
    if(!rd_settypename(address, "PACKED_SWITCH_PAYLOAD", nullptr))
        return nullptr;

    RDType t;
    if(!rdtype_create_n("i32", payload->size, &t)) return nullptr;

    RDAddress tgtaddress = address + rd_nsizeof("PACKED_SWITCH_PAYLOAD");
    if(rd_settype(tgtaddress, &t, nullptr))
        rd_setname(tgtaddress, dalvik::gen_name(tgtaddress, "targets").c_str());

    return &payload->targets[0];
}

const i32* create_sparseswitchpayload(RDAddress address,
                                      const SparseSwitchPayload* payload) {
    if(!rd_settypename(address, "SPARSE_SWITCH_PAYLOAD", nullptr))
        return nullptr;

    RDType t;
    if(!rdtype_create_n("i32", payload->size, &t)) return nullptr;

    usize nsize = rd_tsizeof(&t);
    RDAddress keysaddress = address + rd_nsizeof("SPARSE_SWITCH_PAYLOAD");
    RDAddress tgtaddress = keysaddress + nsize;

    if(rd_settype(keysaddress, &t, nullptr))
        rd_setname(keysaddress, dalvik::gen_name(keysaddress, "keys").c_str());

    if(rd_settype(tgtaddress, &t, nullptr))
        rd_setname(tgtaddress, dalvik::gen_name(tgtaddress, "targets").c_str());

    return reinterpret_cast<const i32*>(
        reinterpret_cast<const u8*>(&payload->keys[0]) + nsize);
}

void create_fillarraydata(RDAddress address, const FillArrayData* fillarray) {
    rd_settypename(address, "FILL_ARRAY_DATA", nullptr);

    RDType t;
    if(!rdtype_create_n(
           "u8", static_cast<usize>(fillarray->size) * fillarray->element_width,
           &t))
        return;

    RDAddress dataaddress = address + rd_nsizeof("FILL_ARRAY_DATA");

    if(rd_settype(dataaddress, &t, nullptr)) {
        rd_setname(dataaddress,
                   dalvik::gen_name(dataaddress, "array_data").c_str());
    }
}

} // namespace dalvik
