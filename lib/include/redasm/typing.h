#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

extern const u32 TID_BOOL;
extern const u32 TID_CHAR;
extern const u32 TID_WCHAR;
extern const u32 TID_U8;
extern const u32 TID_U16;
extern const u32 TID_U32;
extern const u32 TID_U64;
extern const u32 TID_I8;
extern const u32 TID_I16;
extern const u32 TID_I32;
extern const u32 TID_I64;
extern const u32 TID_U16BE;
extern const u32 TID_U32BE;
extern const u32 TID_U64BE;
extern const u32 TID_I16BE;
extern const u32 TID_I32BE;
extern const u32 TID_I64BE;
extern const u32 TID_STR;
extern const u32 TID_WSTR;

typedef struct RDStructField {
    const char* type;
    const char* name;
} RDStructField;

RD_HANDLE(RDValueDict);
RD_HANDLE(RDValueList);

typedef u32 TypeId;

typedef struct RDType {
    TypeId id;
    usize n; // > 0 = array
} RDType;

// clang-format off
typedef struct RDValue {
    RDType type;

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
REDASM_EXPORT bool rd_createtype(const char* tname, RDType* t);
REDASM_EXPORT const char* rd_createstruct(const char* name,
                                          const RDStructField* fields);

inline bool rd_istypenull(const RDType* t) { return t && !t->id; }
