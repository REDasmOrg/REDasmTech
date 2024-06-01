#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

typedef u32 RDByte;

enum RDByteFlags {
    BF_HASBYTE = 1U << 8,
    BF_CONT = 1U << 9,
    BF_NAME = 1U << 10,
    BF_REFS = 1U << 11,
    BF_TYPE = 1U << 12,
    BF_ARRAY = 1U << 13,
    BF_SEGMENT = 1U << 14,
    BF_IMPORT = 1U << 15,
    BF_EXPORT = 1U << 16,

    BF_CALL = 1U << 17,
    BF_JUMP = 1U << 18,
    BF_FLOW = 1U << 19,
    BF_FUNCTION = 1U << 20,
    BF_JUMPDST = 1U << 21,

    BF_UNKNOWN = 0U << 30,
    BF_DATA = 1U << 30,
    BF_CODE = 2U << 30,

    BF_BYTEMASK = 0x000000ff, // Internal use
    BF_UDCMASK = 0xc0000000,  // Internal use
};

REDASM_EXPORT bool rdbyte_isunknown(RDByte self);
REDASM_EXPORT bool rdbyte_iscode(RDByte self);
REDASM_EXPORT bool rdbyte_isdata(RDByte self);
REDASM_EXPORT bool rdbyte_has(RDByte self, usize f);
REDASM_EXPORT bool rdbyte_hasbyte(RDByte self);
REDASM_EXPORT bool rdbyte_getbyte(RDByte self, u8* b);
