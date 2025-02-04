#pragma once

#include <redasm/byte.h>
#include <redasm/types.h>

struct RDBuffer {
    union {
        u8* data;
        RDMByte* m_data;
    };

    usize length;
    const char* source;
    bool (*get_byte)(const struct RDBuffer*, usize, u8*);
};
