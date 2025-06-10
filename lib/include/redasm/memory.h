#pragma once

#include <redasm/byte.h>
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
    RDType type;
    const char* value;
    usize totalsize;
    bool terminated;
} RDStringResult;

REDASM_EXPORT bool rd_getmbyte(RDAddress address, RDMByte* mb);
REDASM_EXPORT void rd_memoryinfo(RDMemoryInfo* mi);
REDASM_EXPORT bool rd_isaddress(RDAddress address);
REDASM_EXPORT bool rd_checkstring(RDAddress address, RDStringResult* r);

REDASM_EXPORT usize rd_read(RDAddress address, void* data, usize n);
REDASM_EXPORT bool rd_getbool(RDAddress address, bool* v);
REDASM_EXPORT bool rd_getchar(RDAddress address, char* v);
REDASM_EXPORT bool rd_getu8(RDAddress address, u8* v);
REDASM_EXPORT bool rd_getu16(RDAddress address, u16* v);
REDASM_EXPORT bool rd_getu32(RDAddress address, u32* v);
REDASM_EXPORT bool rd_getu64(RDAddress address, u64* v);
REDASM_EXPORT bool rd_geti8(RDAddress address, i8* v);
REDASM_EXPORT bool rd_geti16(RDAddress address, i16* v);
REDASM_EXPORT bool rd_geti32(RDAddress address, i32* v);
REDASM_EXPORT bool rd_geti64(RDAddress address, i64* v);
REDASM_EXPORT bool rd_getu16be(RDAddress address, u16* v);
REDASM_EXPORT bool rd_getu32be(RDAddress address, u32* v);
REDASM_EXPORT bool rd_getu64be(RDAddress address, u64* v);
REDASM_EXPORT bool rd_geti16be(RDAddress address, i16* v);
REDASM_EXPORT bool rd_geti32be(RDAddress address, i32* v);
REDASM_EXPORT bool rd_geti64be(RDAddress address, i64* v);
REDASM_EXPORT bool rd_getuleb128(RDAddress address, RDLEB128* v);
REDASM_EXPORT bool rd_getleb128(RDAddress address, RDLEB128* v);
REDASM_EXPORT const char* rd_getstr(RDAddress address, usize n);
REDASM_EXPORT const char* rd_getwstr(RDAddress address, usize n);
REDASM_EXPORT const char* rd_getstrz(RDAddress address);
REDASM_EXPORT const char* rd_getwstrz(RDAddress address);
