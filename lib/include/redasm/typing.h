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

typedef u32 TypeId;

typedef struct RDType {
    TypeId id;
    usize n; // > 0 = array
} RDType;

struct RDValue;
define_slice(RDValueSlice, RDValue);
define_hmap(RDValueDict, 1 << 4);

typedef struct RDValue {
    RDType type;

    RDValueSlice list;
    RDValueDict dict;
    Str str;

    // clang-format off
    union {
        bool b_v; char ch_v;
        u8 u8_v; u16 u16_v; u32 u32_v; u64 u64_v;
        i8 i8_v; i16 i16_v; i32 i32_v; i64 i64_v;
    };
    // clang-format on

    Str _scratchpad;
} RDValue;

define_optional(RDValueOpt, RDValue);

typedef struct RDValueHNode {
    char* key;
    RDValue value;
    HListNode hnode;
} RDValueHNode;

REDASM_EXPORT usize rd_nsizeof(const char* tname);
REDASM_EXPORT usize rd_tsizeof(const RDType* t);
REDASM_EXPORT bool rdtype_create(const char* tname, RDType* t);
REDASM_EXPORT bool rdtype_create_n(const char* tname, usize n, RDType* t);
REDASM_EXPORT bool rd_intfrombytes(usize bytes, bool sign, RDType* t);
REDASM_EXPORT const char* rd_typename(const RDType* t);
REDASM_EXPORT const char* rd_createstruct(const char* name,
                                          const RDStructField* fields);
REDASM_EXPORT void rdvalue_init(RDValue* v);
REDASM_EXPORT void rdvalue_destroy(RDValue* self);
REDASM_EXPORT const char* rdvalue_tostring(RDValue* self);
REDASM_EXPORT bool rdvalue_islist(const RDValue* self);
REDASM_EXPORT bool rdvalue_isstruct(const RDValue* self);
REDASM_EXPORT usize rdvalue_getlength(const RDValue* self);

REDASM_EXPORT const RDValue* rdvalue_query(const RDValue* self, const char* q,
                                           const char** error);

REDASM_EXPORT const RDValue* rdvalue_query_n(const RDValue* self, const char* q,
                                             usize n, const char** error);

inline bool rd_istypenull(const RDType* t) { return t && !t->id; }
