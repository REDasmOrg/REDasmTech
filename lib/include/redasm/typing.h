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

REDASM_EXPORT usize rd_sizeof(const char* tname);
REDASM_EXPORT bool rd_createtype(const char* tname, RDType* t);
REDASM_EXPORT bool rd_createtype_n(const char* tname, usize n, RDType* t);
REDASM_EXPORT const char* rd_typename(const RDType* t);
REDASM_EXPORT const char* rd_createstruct(const char* name,
                                          const RDStructField* fields);

inline bool rd_istypenull(const RDType* t) { return t && !t->id; }

REDASM_EXPORT RDValue* rdvalue_create();
REDASM_EXPORT const RDType* rdvalue_gettype(const RDValue* self);
REDASM_EXPORT const char* rdvalue_tostring(const RDValue* self);
REDASM_EXPORT usize rdvalue_getlength(const RDValue* self);

REDASM_EXPORT bool rdvalue_at(const RDValue* self, usize idx,
                              const RDValue** v);

REDASM_EXPORT bool rdvalue_get(const RDValue* self, const char* key,
                               const RDValue** v);

REDASM_EXPORT bool rdvalue_query(const RDValue* self, const char* q,
                                 const RDValue** v,
                                 const char** error = nullptr);

REDASM_EXPORT bool rdvalue_query_n(const RDValue* self, const char* q, usize n,
                                   const RDValue** v,
                                   const char** error = nullptr);

REDASM_EXPORT bool rdvalue_getu8(const RDValue* self, const char* q, u8* v);
REDASM_EXPORT bool rdvalue_getu16(const RDValue* self, const char* q, u16* v);
REDASM_EXPORT bool rdvalue_getu32(const RDValue* self, const char* q, u32* v);
REDASM_EXPORT bool rdvalue_getu64(const RDValue* self, const char* q, u64* v);
REDASM_EXPORT bool rdvalue_geti8(const RDValue* self, const char* q, i8* v);
REDASM_EXPORT bool rdvalue_geti16(const RDValue* self, const char* q, i16* v);
REDASM_EXPORT bool rdvalue_geti32(const RDValue* self, const char* q, i32* v);
REDASM_EXPORT bool rdvalue_geti64(const RDValue* self, const char* q, i64* v);
REDASM_EXPORT bool rdvalue_getstr(const RDValue* self, const char* q,
                                  const char** v);
