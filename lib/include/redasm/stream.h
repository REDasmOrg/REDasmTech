#pragma once

#include <redasm/buffer.h>
#include <redasm/common.h>
#include <redasm/types.h>
#include <redasm/typing.h>

typedef struct RDStream RDStream;

// clang-format off
REDASM_EXPORT RDStream* rdstream_create(RDBuffer* buffer);
REDASM_EXPORT void rdstream_destroy(RDStream* s);
REDASM_EXPORT usize rdstream_getpos(const RDStream* self);
REDASM_EXPORT usize rdstream_seek(RDStream* self, usize off);
REDASM_EXPORT usize rdstream_move(RDStream* self, isize off);
REDASM_EXPORT void rdstream_rewind(RDStream* self);
REDASM_EXPORT RDValueOpt rdstream_peek_struct_n(RDStream* self, usize n, const RDStructFieldDecl* fields);
REDASM_EXPORT RDValueOpt rdstream_peek_struct(RDStream* self, const RDStructFieldDecl* fields);
REDASM_EXPORT RDValueOpt rdstream_peek_type(const RDStream* self, const char* tname);
REDASM_EXPORT bool rdstream_peek_strz(const RDStream* self, const char** v);
REDASM_EXPORT bool rdstream_peek_str(const RDStream* self, usize n, const char** v);
REDASM_EXPORT bool rdstream_peek_wstrz(const RDStream* self, const char** v);
REDASM_EXPORT bool rdstream_peek_wstr(const RDStream* self, usize n, const char** v);
REDASM_EXPORT bool rdstream_peek_u8(const RDStream* self, u8* v);
REDASM_EXPORT bool rdstream_peek_u16(const RDStream* self, u16* v);
REDASM_EXPORT bool rdstream_peek_u32(const RDStream* self, u32* v);
REDASM_EXPORT bool rdstream_peek_u64(const RDStream* self, u64* v);
REDASM_EXPORT bool rdstream_peek_i8(const RDStream* self, i8* v);
REDASM_EXPORT bool rdstream_peek_i16(const RDStream* self, i16* v);
REDASM_EXPORT bool rdstream_peek_i32(const RDStream* self, i32* v);
REDASM_EXPORT bool rdstream_peek_i64(const RDStream* self, i64* v);
REDASM_EXPORT bool rdstream_peek_u16be(const RDStream* self, u16* v);
REDASM_EXPORT bool rdstream_peek_u32be(const RDStream* self, u32* v);
REDASM_EXPORT bool rdstream_peek_u64be(const RDStream* self, u64* v);
REDASM_EXPORT bool rdstream_peek_i16be(const RDStream* self, i16* v);
REDASM_EXPORT bool rdstream_peek_i32be(const RDStream* self, i32* v);
REDASM_EXPORT bool rdstream_peek_i64be(const RDStream* self, i64* v);
REDASM_EXPORT RDValueOpt rdstream_read_struct_n(RDStream* self, usize n, const RDStructFieldDecl* fields);
REDASM_EXPORT RDValueOpt rdstream_read_struct(RDStream* self, const RDStructFieldDecl* fields);
REDASM_EXPORT RDValueOpt rdstream_read_type(RDStream* self, const char* tname);
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
// clang-format on
