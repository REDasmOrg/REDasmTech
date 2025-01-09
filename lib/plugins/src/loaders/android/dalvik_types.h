#pragma once

#include <redasm/redasm.h>

namespace dalvik {

const RDStructField PACKEDSWITCHPAYLOAD_TYPE[] = {
    {"u16", "ident"},
    {"u16", "size"},
    {"i32", "first_key"},
    {nullptr, nullptr},
};

struct PackedSwitchPayload {
    u16 ident, size;
    i32 first_key;
    i32 targets[1];
};

void create_packedswitchpayload(RDAddress address,
                                const PackedSwitchPayload* payload);

} // namespace dalvik
