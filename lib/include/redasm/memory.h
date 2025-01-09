#pragma once

#include <redasm/common.h>
#include <redasm/types.h>
#include <redasm/typing.h>

struct RDMemoryInfo {
    union {
        RDAddress start;
        RDAddress baseaddress;
    };

    union {
        RDAddress end;
        RDAddress end_baseaddress;
    };

    usize size;
};

typedef struct RDStringResult {
    const char* type;
    const char* value;
    usize totalsize;
    bool terminated;
} RDStringResult;

REDASM_EXPORT void rd_memoryinfo(RDMemoryInfo* mi);
REDASM_EXPORT bool rd_map(RDAddress startaddr, RDAddress endaddr);
REDASM_EXPORT bool rd_map_n(RDAddress baseaddress, usize size);
REDASM_EXPORT bool rd_isaddress(RDAddress address);

REDASM_EXPORT usize rd_read(RDAddress address, void* data, usize n);
REDASM_EXPORT bool rd_getbool(RDAddress address, bool* ok);
REDASM_EXPORT char rd_getchar(RDAddress address, bool* ok);
REDASM_EXPORT u8 rd_getu8(RDAddress address, bool* ok);
REDASM_EXPORT u16 rd_getu16(RDAddress address, bool* ok);
REDASM_EXPORT u32 rd_getu32(RDAddress address, bool* ok);
REDASM_EXPORT u64 rd_getu64(RDAddress address, bool* ok);
REDASM_EXPORT i8 rd_geti8(RDAddress address, bool* ok);
REDASM_EXPORT i16 rd_geti16(RDAddress address, bool* ok);
REDASM_EXPORT i32 rd_geti32(RDAddress address, bool* ok);
REDASM_EXPORT i64 rd_geti64(RDAddress address, bool* ok);
REDASM_EXPORT u16 rd_getu16be(RDAddress address, bool* ok);
REDASM_EXPORT u32 rd_getu32be(RDAddress address, bool* ok);
REDASM_EXPORT u64 rd_getu64be(RDAddress address, bool* ok);
REDASM_EXPORT i16 rd_geti16be(RDAddress address, bool* ok);
REDASM_EXPORT i32 rd_geti32be(RDAddress address, bool* ok);
REDASM_EXPORT i64 rd_geti64be(RDAddress address, bool* ok);
REDASM_EXPORT const char* rd_getstr(RDAddress address, usize n);
REDASM_EXPORT const char* rd_getwstr(RDAddress address, usize n);
REDASM_EXPORT const char* rd_getstrz(RDAddress address);
REDASM_EXPORT const char* rd_getwstrz(RDAddress address);
