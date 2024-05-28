#pragma once

#include "../../typing/typing.h"
#include <redasm/stream.h>
#include <string>
#include <tl/optional.hpp>

namespace redasm::api::internal {

RDStream* stream_createfromfile();
RDStream* stream_createfrommemory();
usize stream_seek(RDStream* self, usize off);
usize stream_move(RDStream* self, isize off);
usize stream_getposition(const RDStream* self);
void stream_rewind(RDStream* self);

tl::optional<typing::Value> stream_peek_type(const RDStream* self,
                                             std::string_view tname);
tl::optional<std::string> stream_peek_stringz(const RDStream* self);
tl::optional<std::string> stream_peek_string(const RDStream* self, usize n);
tl::optional<u8> stream_peek_u8(const RDStream* self);
tl::optional<u16> stream_peek_u16(const RDStream* self);
tl::optional<u32> stream_peek_u32(const RDStream* self);
tl::optional<u64> stream_peek_u64(const RDStream* self);
tl::optional<i8> stream_peek_i8(const RDStream* self);
tl::optional<i16> stream_peek_i16(const RDStream* self);
tl::optional<i32> stream_peek_i32(const RDStream* self);
tl::optional<i64> stream_peek_i64(const RDStream* self);
tl::optional<u16> stream_peek_u16be(const RDStream* self);
tl::optional<u32> stream_peek_u32be(const RDStream* self);
tl::optional<u64> stream_peek_u64be(const RDStream* self);
tl::optional<i16> stream_peek_i16be(const RDStream* self);
tl::optional<i32> stream_peek_i32be(const RDStream* self);
tl::optional<i64> stream_peek_i64be(const RDStream* self);

tl::optional<typing::Value> stream_read_type(RDStream* self,
                                             std::string_view tname);
tl::optional<std::string> stream_read_stringz(RDStream* self);
tl::optional<std::string> stream_read_string(RDStream* self, usize n);
tl::optional<u8> stream_read_u8(RDStream* self);
tl::optional<u16> stream_read_u16(RDStream* self);
tl::optional<u32> stream_read_u32(RDStream* self);
tl::optional<u64> stream_read_u64(RDStream* self);
tl::optional<i8> stream_read_i8(RDStream* self);
tl::optional<i16> stream_read_i16(RDStream* self);
tl::optional<i32> stream_read_i32(RDStream* self);
tl::optional<i64> stream_read_i64(RDStream* self);
tl::optional<u16> stream_read_u16be(RDStream* self);
tl::optional<u32> stream_read_u32be(RDStream* self);
tl::optional<u64> stream_read_u64be(RDStream* self);
tl::optional<i16> stream_read_i16be(RDStream* self);
tl::optional<i32> stream_read_i32be(RDStream* self);
tl::optional<i64> stream_read_i64be(RDStream* self);

tl::optional<typing::Value> stream_collect_type(RDStream* self,
                                                std::string_view tname);
tl::optional<std::string> stream_collect_stringz(RDStream* self);
tl::optional<std::string> stream_collect_string(RDStream* self, usize n);

} // namespace redasm::api::internal
