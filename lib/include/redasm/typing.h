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

RD_HANDLE(RDValue);
RD_HANDLE(RDValueDict);
RD_HANDLE(RDValueList);

typedef u32 TypeId;

typedef struct RDType {
    TypeId id;
    usize n; // > 0 = array
} RDType;

REDASM_EXPORT usize rd_nsizeof(const char* tname);
REDASM_EXPORT usize rd_tsizeof(const RDType* t);
REDASM_EXPORT bool rdtype_create(const char* tname, RDType* t);
REDASM_EXPORT bool rdtype_create_n(const char* tname, usize n, RDType* t);
REDASM_EXPORT bool rd_intfrombytes(usize bytes, bool sign, RDType* t);
REDASM_EXPORT const char* rd_typename(const RDType* t);
REDASM_EXPORT const char* rd_createstruct(const char* name,
                                          const RDStructField* fields);

inline bool rd_istypenull(const RDType* t) { return t && !t->id; }

REDASM_EXPORT RDValue* rdvalue_create();
REDASM_EXPORT void rdvalue_destroy(RDValue* v);
REDASM_EXPORT const RDType* rdvalue_gettype(const RDValue* self);
REDASM_EXPORT const char* rdvalue_tostring(const RDValue* self);
REDASM_EXPORT usize rdvalue_getlength(const RDValue* self);

REDASM_EXPORT const RDValue* rdvalue_at(const RDValue* self, usize idx);

REDASM_EXPORT const RDValue* rdvalue_get(const RDValue* self, const char* q,
                                         const char** error);

REDASM_EXPORT const RDValue* rdvalue_get_n(const RDValue* self, const char* q,
                                           usize n, const char** error);

REDASM_EXPORT u8 rdvalue_getu8(const RDValue* self, const char* q, bool* ok);
REDASM_EXPORT u16 rdvalue_getu16(const RDValue* self, const char* q, bool* ok);
REDASM_EXPORT u32 rdvalue_getu32(const RDValue* self, const char* q, bool* ok);
REDASM_EXPORT u64 rdvalue_getu64(const RDValue* self, const char* q, bool* ok);
REDASM_EXPORT i8 rdvalue_geti8(const RDValue* self, const char* q, bool* ok);
REDASM_EXPORT i16 rdvalue_geti16(const RDValue* self, const char* q, bool* ok);
REDASM_EXPORT i32 rdvalue_geti32(const RDValue* self, const char* q, bool* ok);
REDASM_EXPORT i64 rdvalue_geti64(const RDValue* self, const char* q, bool* ok);
REDASM_EXPORT const char* rdvalue_getstr(const RDValue* self, const char* q,
                                         bool* ok);
