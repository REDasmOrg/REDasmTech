#pragma once

#include <redasm/common.h>
#include <redasm/types.h>
#include <redasm/typing.h>

RD_HANDLE(RDStream);

REDASM_EXPORT RDStream* rdstream_createfromfile();
REDASM_EXPORT RDStream* rdstream_createfrommemory();

REDASM_EXPORT usize rdstream_seek(RDStream* self, usize off);
REDASM_EXPORT usize rdstream_move(RDStream* self, isize off);
REDASM_EXPORT usize rdstream_getpos(const RDStream* self);
REDASM_EXPORT void rdstream_rewind(RDStream* self);

REDASM_EXPORT bool rdstream_peek_type(RDStream* self, const char* tname,
                                      RDValue* v);
REDASM_EXPORT bool rdstream_peek_strz(RDStream* self, const char** v);
REDASM_EXPORT bool rdstream_peek_str(RDStream* self, usize n, const char** v);
REDASM_EXPORT bool rdstream_peek_wstrz(RDStream* self, const char** v);
REDASM_EXPORT bool rdstream_peek_wstr(RDStream* self, usize n, const char** v);
REDASM_EXPORT bool rdstream_peek_u8(RDStream* self, u8* v);
REDASM_EXPORT bool rdstream_peek_u16(RDStream* self, u16* v);
REDASM_EXPORT bool rdstream_peek_u32(RDStream* self, u32* v);
REDASM_EXPORT bool rdstream_peek_u64(RDStream* self, u64* v);
REDASM_EXPORT bool rdstream_peek_i8(RDStream* self, i8* v);
REDASM_EXPORT bool rdstream_peek_i16(RDStream* self, i16* v);
REDASM_EXPORT bool rdstream_peek_i32(RDStream* self, i32* v);
REDASM_EXPORT bool rdstream_peek_i64(RDStream* self, i64* v);
REDASM_EXPORT bool rdstream_peek_u16be(RDStream* self, u16* v);
REDASM_EXPORT bool rdstream_peek_u32be(RDStream* self, u32* v);
REDASM_EXPORT bool rdstream_peek_u64be(RDStream* self, u64* v);
REDASM_EXPORT bool rdstream_peek_i16be(RDStream* self, i16* v);
REDASM_EXPORT bool rdstream_peek_i32be(RDStream* self, i32* v);
REDASM_EXPORT bool rdstream_peek_i64be(RDStream* self, i64* v);

REDASM_EXPORT bool rdstream_read_type(RDStream* self, const char* tname,
                                      RDValue* v);
REDASM_EXPORT bool rdstream_read_strz(RDStream* self, const char** v);
REDASM_EXPORT bool rdstream_read_str(RDStream* self, usize n, const char** v);
REDASM_EXPORT bool rdstream_read_wstrz(RDStream* self, const char** v);
REDASM_EXPORT bool rdstream_read_wstr(RDStream* self, usize n, const char** v);
REDASM_EXPORT bool rdstream_read_u8(RDStream* self, u8* v);
REDASM_EXPORT bool rdstream_read_u16(RDStream* self, u16* v);
REDASM_EXPORT bool rdstream_read_u32(RDStream* self, u32* v);
REDASM_EXPORT bool rdstream_read_u64(RDStream* self, u64* v);
REDASM_EXPORT bool rdstream_read_i8(RDStream* self, i8* v);
REDASM_EXPORT bool rdstream_read_i16(RDStream* self, i16* v);
REDASM_EXPORT bool rdstream_read_i32(RDStream* self, i32* v);
REDASM_EXPORT bool rdstream_read_i64(RDStream* self, i64* v);
REDASM_EXPORT bool rdstream_read_u16be(RDStream* self, u16* v);
REDASM_EXPORT bool rdstream_read_u32be(RDStream* self, u32* v);
REDASM_EXPORT bool rdstream_read_u64be(RDStream* self, u64* v);
REDASM_EXPORT bool rdstream_read_i16be(RDStream* self, i16* v);
REDASM_EXPORT bool rdstream_read_i32be(RDStream* self, i32* v);
REDASM_EXPORT bool rdstream_read_i64be(RDStream* self, i64* v);

REDASM_EXPORT bool rdstream_collect_type(RDStream* self, const char* tname,
                                         RDValue* v);
REDASM_EXPORT bool rdstream_collect_strz(RDStream* self, const char** v);
REDASM_EXPORT bool rdstream_collect_str(RDStream* self, usize n,
                                        const char** v);
REDASM_EXPORT bool rdstream_collect_wstrz(RDStream* self, const char** v);
REDASM_EXPORT bool rdstream_collect_wstr(RDStream* self, usize n,
                                         const char** v);
