#pragma once

#include "../../typing/value.h"
#include <redasm/redasm.h>
#include <string>
#include <string_view>
#include <tl/optional.hpp>

namespace redasm::api::internal {

RDBuffer* buffer_getfile();
RDBuffer* buffer_getmemory();
usize buffer_read(const RDBuffer* self, usize idx, void* dst, usize n);
tl::optional<bool> buffer_getbool(const RDBuffer* self, usize idx);
tl::optional<char> buffer_getchar(const RDBuffer* self, usize idx);
tl::optional<u8> buffer_getu8(const RDBuffer* self, usize idx);
tl::optional<u16> buffer_getu16(const RDBuffer* self, usize idx);
tl::optional<u32> buffer_getu32(const RDBuffer* self, usize idx);
tl::optional<u64> buffer_getu64(const RDBuffer* self, usize idx);
tl::optional<i8> buffer_geti8(const RDBuffer* self, usize idx);
tl::optional<i16> buffer_geti16(const RDBuffer* self, usize idx);
tl::optional<i32> buffer_geti32(const RDBuffer* self, usize idx);
tl::optional<i64> buffer_geti64(const RDBuffer* self, usize idx);
tl::optional<u16> buffer_getu16be(const RDBuffer* self, usize idx);
tl::optional<u32> buffer_getu32be(const RDBuffer* self, usize idx);
tl::optional<u64> buffer_getu64be(const RDBuffer* self, usize idx);
tl::optional<i16> buffer_geti16be(const RDBuffer* self, usize idx);
tl::optional<i32> buffer_geti32be(const RDBuffer* self, usize idx);
tl::optional<i64> buffer_geti64be(const RDBuffer* self, usize idx);
tl::optional<std::string> buffer_getstrz(const RDBuffer* self, usize idx);
tl::optional<std::string> buffer_getstr(const RDBuffer* self, usize idx,
                                        usize n);
tl::optional<std::string> buffer_getwstrz(const RDBuffer* self, usize idx);
tl::optional<std::string> buffer_getwstr(const RDBuffer* self, usize idx,
                                         usize n);
tl::optional<typing::Value> buffer_gettype(const RDBuffer* self, usize idx,
                                           const RDType* t);
tl::optional<typing::Value> buffer_gettypename(const RDBuffer* self, usize idx,
                                               std::string_view tname);
tl::optional<typing::Value> buffer_collecttype(const RDBuffer* self, usize idx,
                                               const RDType* t);
tl::optional<typing::Value>
buffer_collecttypename(const RDBuffer* self, usize idx, std::string_view tname);

} // namespace redasm::api::internal
