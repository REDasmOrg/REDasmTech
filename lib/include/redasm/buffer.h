#pragma once

#include <redasm/byte.h>
#include <redasm/common.h>
#include <redasm/types.h>
#include <redasm/typing.h>

typedef struct RDBuffer {
    union {
        u8* data;
        RDMByte* m_data;
    };

    usize length;
    const char* source;
    bool (*get_byte)(const struct RDBuffer*, usize, u8*);
} RDBuffer;

REDASM_EXPORT RDBuffer* rdbuffer_createfile(const char* filepath);
REDASM_EXPORT RDBuffer* rdbuffer_creatememory(usize n);
REDASM_EXPORT usize rdbuffer_getlength(const RDBuffer* self);
REDASM_EXPORT const char* rdbuffer_getsource(const RDBuffer* self);
REDASM_EXPORT const u8* rdbuffer_getdata(const RDBuffer* self);
REDASM_EXPORT const RDMByte* rdbuffer_getmdata(const RDBuffer* self);
REDASM_EXPORT usize rdbuffer_read(const RDBuffer* self, usize idx, void* dst,
                                  usize n);
REDASM_EXPORT RDValueOpt rdbuffer_readstruct_n(const RDBuffer* self, usize idx,
                                               usize n,
                                               const RDStructField* fields);
REDASM_EXPORT RDValueOpt rdbuffer_readstruct(const RDBuffer* self, usize idx,
                                             const RDStructField* fields);
REDASM_EXPORT bool rdbuffer_getmbyte(const RDBuffer* self, usize idx,
                                     RDMByte* v);
REDASM_EXPORT bool rdbuffer_getbool(const RDBuffer* self, usize idx, bool* v);
REDASM_EXPORT bool rdbuffer_getchar(const RDBuffer* self, usize idx, char* v);
REDASM_EXPORT bool rdbuffer_getu8(const RDBuffer* self, usize idx, u8* v);
REDASM_EXPORT bool rdbuffer_getu16(const RDBuffer* self, usize idx, u16* v);
REDASM_EXPORT bool rdbuffer_getu32(const RDBuffer* self, usize idx, u32* v);
REDASM_EXPORT bool rdbuffer_getu64(const RDBuffer* self, usize idx, u64* v);
REDASM_EXPORT bool rdbuffer_geti8(const RDBuffer* self, usize idx, i8* v);
REDASM_EXPORT bool rdbuffer_geti16(const RDBuffer* self, usize idx, i16* v);
REDASM_EXPORT bool rdbuffer_geti32(const RDBuffer* self, usize idx, i32* v);
REDASM_EXPORT bool rdbuffer_geti64(const RDBuffer* self, usize idx, i64* v);
REDASM_EXPORT bool rdbuffer_getu16be(const RDBuffer* self, usize idx, u16* v);
REDASM_EXPORT bool rdbuffer_getu32be(const RDBuffer* self, usize idx, u32* v);
REDASM_EXPORT bool rdbuffer_getu64be(const RDBuffer* self, usize idx, u64* v);
REDASM_EXPORT bool rdbuffer_geti16be(const RDBuffer* self, usize idx, i16* v);
REDASM_EXPORT bool rdbuffer_geti32be(const RDBuffer* self, usize idx, i32* v);
REDASM_EXPORT bool rdbuffer_geti64be(const RDBuffer* self, usize idx, i64* v);
REDASM_EXPORT bool rdbuffer_getstrz(const RDBuffer* self, usize idx,
                                    const char** v);
REDASM_EXPORT bool rdbuffer_getstr(const RDBuffer* self, usize idx, usize n,
                                   const char** v);
REDASM_EXPORT bool rdbuffer_getwstrz(const RDBuffer* self, usize idx,
                                     const char** v);
REDASM_EXPORT bool rdbuffer_getwstr(const RDBuffer* self, usize idx, usize n,
                                    const char** v);
REDASM_EXPORT RDValueOpt rdbuffer_gettypename(const RDBuffer* self, usize idx,
                                              const char* tname);
REDASM_EXPORT RDValueOpt rdbuffer_gettype(const RDBuffer* self, usize idx,
                                          const RDType* t);
REDASM_EXPORT void rdbuffer_destroy(RDBuffer* self);
