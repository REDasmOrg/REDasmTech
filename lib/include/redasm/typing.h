#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

typedef struct RDStructField {
    const char* type;
    const char* name;
} RDStructField;

RD_HANDLE(RDValueDict);
RD_HANDLE(RDValueList);

// clang-format off
typedef struct RDValue {
    const char* type;
    usize count;

    const RDValueList* list;
    usize listcount;

    const RDValueDict* dict;
    usize dictcount;

    const char* str;

    union {
        bool b_v; char ch_v;
        u8 u8_v; u16 u16_v; u32 u32_v; u64 u64_v;
        i8 i8_v; i16 i16_v; i32 i32_v; i64 i64_v;
    };
} RDValue;
// clang-format on

REDASM_EXPORT usize rd_sizeof(const char* tname);
REDASM_EXPORT const char* rd_createstruct(const char* name,
                                          const RDStructField* fields, usize n);
