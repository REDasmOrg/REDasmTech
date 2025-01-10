#pragma once

#include <redasm/redasm.h>

namespace dalvik {

const RDStructField PACKEDSWITCHPAYLOAD_TYPE[] = {
    {"u16", "ident"},
    {"u16", "size"},
    {"i32", "first_key"},
    {nullptr, nullptr},
};

const RDStructField SPARSESWITCHPAYLOAD_TYPE[] = {
    {"u16", "ident"},
    {"u16", "size"},
    {nullptr, nullptr},
};

const RDStructField FILLARRAYDATA_TYPE[] = {
    {"u16", "ident"},
    {"u16", "element_size"},
    {"u32", "size"},
    {nullptr, nullptr},
};

#pragma pack(push, 1)
struct PackedSwitchPayload {
    u16 ident;
    u16 size;
    i32 first_key;
    i32 targets[1];
};

struct SparseSwitchPayload {
    u16 ident;
    u16 size;
    i32 keys[1];
    // i32 targets[1];
};

struct FillArrayData {
    u16 ident;
    u16 element_width;
    u32 size;
    u8 data[1];
};
#pragma pack(pop)

const i32* create_packedswitchpayload(RDAddress address,
                                      const PackedSwitchPayload* payload);

const i32* create_sparseswitchpayload(RDAddress address,
                                      const SparseSwitchPayload* payload);

void create_fillarraydata(RDAddress address, const FillArrayData* fillarray);

} // namespace dalvik
