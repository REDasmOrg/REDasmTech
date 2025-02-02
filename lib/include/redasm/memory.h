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
REDASM_EXPORT bool rd_isaddress(RDAddress address);
REDASM_EXPORT bool rd_checkstring(RDAddress address, RDStringResult* r);

REDASM_EXPORT usize rd_read(RDAddress address, void* data, usize n);
REDASM_EXPORT bool rd_getbool(RDAddress address, bool* v);
REDASM_EXPORT bool rd_getchar(RDAddress address, char* ok);
REDASM_EXPORT bool rd_getu8(RDAddress address, u8* ok);
REDASM_EXPORT bool rd_getu16(RDAddress address, u16* ok);
REDASM_EXPORT bool rd_getu32(RDAddress address, u32* ok);
REDASM_EXPORT bool rd_getu64(RDAddress address, u64* ok);
REDASM_EXPORT bool rd_geti8(RDAddress address, i8* ok);
REDASM_EXPORT bool rd_geti16(RDAddress address, i16* ok);
REDASM_EXPORT bool rd_geti32(RDAddress address, i32* ok);
REDASM_EXPORT bool rd_geti64(RDAddress address, i64* ok);
REDASM_EXPORT bool rd_getu16be(RDAddress address, u16* ok);
REDASM_EXPORT bool rd_getu32be(RDAddress address, u32* ok);
REDASM_EXPORT bool rd_getu64be(RDAddress address, u64* ok);
REDASM_EXPORT bool rd_geti16be(RDAddress address, i16* ok);
REDASM_EXPORT bool rd_geti32be(RDAddress address, i32* ok);
REDASM_EXPORT bool rd_geti64be(RDAddress address, i64* ok);
REDASM_EXPORT const char* rd_getstr(RDAddress address, usize n);
REDASM_EXPORT const char* rd_getwstr(RDAddress address, usize n);
REDASM_EXPORT const char* rd_getstrz(RDAddress address);
REDASM_EXPORT const char* rd_getwstrz(RDAddress address);
