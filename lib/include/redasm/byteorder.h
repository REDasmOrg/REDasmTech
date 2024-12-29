#pragma once

#include <redasm/common.h>
#include <redasm/types.h>
#include <sys/param.h>

#if defined(__BYTE_ORDER)
#if defined(__BIG_ENDIAN) && (__BYTE_ORDER == __BIG_ENDIAN)
#define RD_BIGENDIAN_PLATFORM
#elif defined(__LITTLE_ENDIAN) && (__BYTE_ORDER == __LITTLE_ENDIAN)
#define RD_LITTLEENDIAN_PLATFORM
#else
#error "byteorder: unsupported endianness"
#endif
#else
#error "byteorder: cannot detect byte order"
#endif

enum RDByteOrder {
    BO_LITTLE = 0,
    BO_BIG,

#if defined(RD_BIGENDIAN_PLATFORM)
    BO_PLATFORM = BO_BIG,
#else
    BO_PLATFORM = BO_LITTLE,
#endif
};

REDASM_EXPORT u16 rd_fromle16(u16 hostval);
REDASM_EXPORT u32 rd_fromle32(u32 hostval);
REDASM_EXPORT u64 rd_fromle64(u64 hostval);
REDASM_EXPORT u16 rd_frombe16(u16 hostval);
REDASM_EXPORT u32 rd_frombe32(u32 hostval);
REDASM_EXPORT u64 rd_frombe64(u64 hostval);
REDASM_EXPORT u16 rd_tole16(u16 hostval);
REDASM_EXPORT u32 rd_tole32(u32 hostval);
REDASM_EXPORT u64 rd_tole64(u64 hostval);
REDASM_EXPORT u16 rd_tobe16(u16 hostval);
REDASM_EXPORT u32 rd_tobe2(u32 hostval);
REDASM_EXPORT u64 rd_tobe64(u64 hostval);
