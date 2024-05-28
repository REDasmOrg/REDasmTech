#include "buffer.h"
#include "../../memory/abstractbuffer.h"
#include "../marshal.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

tl::optional<bool> buffer_getbool(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getbool({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_bool(idx);
}

tl::optional<char> buffer_getchar(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getchar({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_char(idx);
}

tl::optional<u8> buffer_getu8(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getu8({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_u8(idx);
}

tl::optional<u16> buffer_getu16(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getu16({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_u16(idx, false);
}

tl::optional<u32> buffer_getu32(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getu32({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_u32(idx, false);
}

tl::optional<u64> buffer_getu64(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getu64({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_u64(idx, false);
}

tl::optional<i8> buffer_geti8(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_geti8({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_i8(idx);
}

tl::optional<i16> buffer_geti16(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_geti16({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_i16(idx, false);
}

tl::optional<i32> buffer_geti32(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_geti32({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_i32(idx, false);
}

tl::optional<i64> buffer_geti64(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_geti64({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_i64(idx, false);
}

tl::optional<u16> buffer_getu16be(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getu16be({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_u16(idx, true);
}

tl::optional<u32> buffer_getu32be(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getu32be({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_u32(idx, true);
}

tl::optional<u64> buffer_getu64be(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getu64be({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_u64(idx, true);
}

tl::optional<i16> buffer_geti16be(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_geti16be({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_i16(idx, true);
}

tl::optional<i32> buffer_geti32be(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_geti32be({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_i32(idx, true);
}

tl::optional<i64> buffer_geti64be(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_geti64be({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_i64(idx, true);
}

tl::optional<std::string> buffer_getstringz(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getstringz({}, {})", fmt::ptr(self), idx);
    return api::from_c(self)->get_string(idx);
}

tl::optional<std::string> buffer_getstring(const RDBuffer* self, usize idx,
                                           usize n) {
    spdlog::trace("buffer_getstring({}, {}, {})", fmt::ptr(self), idx, n);
    return api::from_c(self)->get_string(idx, n);
}

tl::optional<typing::Value> buffer_gettype(const RDBuffer* self, usize idx,
                                           std::string_view tname) {
    spdlog::trace("buffer_gettype({}, {}, '{}')", fmt::ptr(self), idx, tname);
    return api::from_c(self)->get_type(idx, tname);
}

} // namespace redasm::api::internal
