#pragma once

#include <redasm/common.h>
#include <redasm/types.h>
#include <redasm/typing.h>

RD_HANDLE(RDBuffer);

REDASM_EXPORT RDBuffer* rdbuffer_getfile(void);
REDASM_EXPORT RDBuffer* rdbuffer_getmemory(void);
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
REDASM_EXPORT bool rdbuffer_gettype(const RDBuffer* self, usize idx,
                                    const char* tname, RDValue* v);
REDASM_EXPORT bool rdbuffer_collecttype(const RDBuffer* self, usize idx,
                                        const char* tname, RDValue* v);
