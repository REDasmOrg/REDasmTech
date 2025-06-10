#pragma once

#include <redasm/common.h>
#include <redasm/ct.h>
#include <redasm/types.h>

struct RDLEB128 {
    usize size;

    union {
        u64 u_val;
        i64 i_val;
    };
};

typedef enum RDTypeKind {
    TK_PRIMITIVE = 0,
    TK_STRUCT,
    TK_FUNCTION,
} RDTypeKind;

typedef enum RDTypeFlags {
    TF_BIG = (1 << 0),
    TF_VAR = (1 << 1),
    TF_INT = (1 << 2),
    TF_SIGN = (1 << 3),
} RDTypeFlags;

// clang-format off
typedef enum RDPrimitiveTypes {
    T_BOOL = 0,
    T_CHAR, T_WCHAR,
    T_U8, T_U16, T_U32, T_U64,
    T_I8, T_I16, T_I32, T_I64,
    T_U16BE, T_U32BE, T_U64BE,
    T_I16BE, T_I32BE, T_I64BE,
    T_STR, T_WSTR,
    T_ULEB128, T_LEB128,

    T_NPRIMITIVES,
} RDPrimitiveType;
// clang-format on

typedef struct RDType RDType;
typedef struct RDTypeDef RDTypeDef;

typedef struct RDType {
    usize n;
    const RDTypeDef* def;
} RDType;

struct RDValue;
define_slice(RDValueSlice, RDValue);
define_hmap(RDValueDict, 1 << 4);

typedef struct RDValue {
    RDType type;
    RDValueSlice list;
    RDValueDict dict;
    Str str;
    RDLEB128 leb;

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

typedef struct RDStructFieldDecl {
    const char* type;
    const char* name;
} RDStructFieldDecl;

typedef struct RDStructField {
    RDType type;
    const char* name;
} RDStructField;

define_slice(RDStructType, RDStructField);

typedef struct RDFunctionType {
    RDType ret;
    const RDType* args;
} RDFunctionType;

typedef struct RDTypeDef {
    RDTypeKind kind;
    const char* name;
    u32 size;
    u32 flags;

    union {
        RDPrimitiveType t_primitive;
        RDStructType t_struct;
        RDFunctionType t_function;
    };
} RDTypeDef;

// clang-format off
REDASM_EXPORT const RDTypeDef* rd_createstruct(const char* name, const RDStructFieldDecl* fields);
REDASM_EXPORT const char* rd_typename(const RDType* t);
REDASM_EXPORT usize rd_nsizeof(const char* tname);
REDASM_EXPORT usize rd_tsizeof(const RDType* t);
REDASM_EXPORT bool rd_createprimitive(RDPrimitiveType pt, RDType* t);
REDASM_EXPORT bool rd_createprimitive_n(RDPrimitiveType pt, usize n, RDType* t);
REDASM_EXPORT bool rd_createtype(const char* tname, RDType* t);
REDASM_EXPORT bool rd_createtype_n(const char* tname, usize n, RDType* t);
REDASM_EXPORT bool rd_settypename(RDAddress address, const char* tname, RDValue* v);
REDASM_EXPORT bool rd_settypename_ex(RDAddress address, const char* tname, usize flags, RDValue* v);
REDASM_EXPORT bool rd_intfrombytes(usize bytes, bool sign, RDType* t);
REDASM_EXPORT bool rd_typeequals(const RDType* t1, const RDType* t2);

REDASM_EXPORT void rdvalue_init(RDValue* v);
REDASM_EXPORT void rdvalue_destroy(RDValue* self);
REDASM_EXPORT const char* rdvalue_tostring(RDValue* self);
REDASM_EXPORT bool rdvalue_islist(const RDValue* self);
REDASM_EXPORT bool rdvalue_isstruct(const RDValue* self);
REDASM_EXPORT usize rdvalue_getlength(const RDValue* self);
REDASM_EXPORT bool rdvalue_getunsigned(const RDValue* self, u64* v);
REDASM_EXPORT bool rdvalue_getsigned(const RDValue* self, i64* v);
REDASM_EXPORT bool rdvalue_getinteger(const RDValue* self, u64* v);
REDASM_EXPORT const RDValue* rdvalue_query(const RDValue* self, const char* q, const char** error);
REDASM_EXPORT const RDValue* rdvalue_query_n(const RDValue* self, const char* q, usize n, const char** error);
// clang-format on

static inline bool rd_typeisnull(const RDType* t) { return !t || !t->def; }
