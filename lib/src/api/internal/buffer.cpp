#include "buffer.h"
#include "../../context.h"
#include "../../memory/abstractbuffer.h"
#include "../../state.h"
#include "../marshal.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

RDBuffer* buffer_getfile() {
    spdlog::trace("buffer_getfile()");
    if(state::context) return api::to_c(state::context->file.get());
    return nullptr;
}

RDBuffer* buffer_getmemory() {
    spdlog::trace("buffer_getmemory()");
    if(state::context) return api::to_c(state::context->memory.get());
    return nullptr;
}

usize buffer_getdata(const RDBuffer* self, const u8** data) {
    spdlog::trace("buffer_getdata({}, {})", fmt::ptr(self), fmt::ptr(data));
    const AbstractBuffer* b = api::from_c(self);
    if(data) *data = b->raw_data();
    return b->size();
}

usize buffer_getlength(const RDBuffer* self) {
    spdlog::trace("buffer_getlength({})", fmt::ptr(self));
    return api::from_c(self)->size();
}

usize buffer_read(const RDBuffer* self, usize idx, void* dst, usize n) {
    spdlog::trace("buffer_read({}, {:x}, {}, {})", fmt::ptr(self), idx,
                  fmt::ptr(dst), n);
    const AbstractBuffer* b = api::from_c(self);
    return b->read(idx, dst, n);
}

tl::optional<bool> buffer_getbool(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getbool({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_bool(idx);
}

tl::optional<char> buffer_getchar(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getchar({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_char(idx);
}

tl::optional<u8> buffer_getu8(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getu8({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_u8(idx);
}

tl::optional<u16> buffer_getu16(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getu16({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_u16(idx, false);
}

tl::optional<u32> buffer_getu32(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getu32({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_u32(idx, false);
}

tl::optional<u64> buffer_getu64(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getu64({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_u64(idx, false);
}

tl::optional<i8> buffer_geti8(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_geti8({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_i8(idx);
}

tl::optional<i16> buffer_geti16(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_geti16({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_i16(idx, false);
}

tl::optional<i32> buffer_geti32(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_geti32({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_i32(idx, false);
}

tl::optional<i64> buffer_geti64(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_geti64({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_i64(idx, false);
}

tl::optional<u16> buffer_getu16be(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getu16be({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_u16(idx, true);
}

tl::optional<u32> buffer_getu32be(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getu32be({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_u32(idx, true);
}

tl::optional<u64> buffer_getu64be(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getu64be({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_u64(idx, true);
}

tl::optional<i16> buffer_geti16be(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_geti16be({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_i16(idx, true);
}

tl::optional<i32> buffer_geti32be(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_geti32be({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_i32(idx, true);
}

tl::optional<i64> buffer_geti64be(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_geti64be({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_i64(idx, true);
}

tl::optional<std::string> buffer_getstrz(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getstrz({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_str(idx);
}

tl::optional<std::string> buffer_getstr(const RDBuffer* self, usize idx,
                                        usize n) {
    spdlog::trace("buffer_getstr({}, {:x}, {})", fmt::ptr(self), idx, n);
    return api::from_c(self)->get_str(idx, n);
}

tl::optional<std::string> buffer_getwstrz(const RDBuffer* self, usize idx) {
    spdlog::trace("buffer_getwstrz({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_wstr(idx);
}

tl::optional<std::string> buffer_getwstr(const RDBuffer* self, usize idx,
                                         usize n) {
    spdlog::trace("buffer_getwstr({}, {:x})", fmt::ptr(self), idx);
    return api::from_c(self)->get_wstr(idx, n);
}

tl::optional<typing::Value> buffer_readstruct(const RDBuffer* self, usize idx,
                                              const typing::Struct& t) {
    spdlog::trace("buffer_readstruct({}, {:x}, <{} fields>)", fmt::ptr(self),
                  idx, t.size());
    return api::from_c(self)->read_struct(idx, t);
}

tl::optional<typing::Value> buffer_gettype(const RDBuffer* self, usize idx,
                                           const RDType* t) {
    spdlog::trace("buffer_gettype({}, {:x}, {})", fmt::ptr(self), idx,
                  fmt::ptr(t));
    if(t) return api::from_c(self)->get_type(idx, *t);
    return tl::nullopt;
}

tl::optional<typing::Value> buffer_gettypename(const RDBuffer* self, usize idx,
                                               std::string_view tname) {
    spdlog::trace("buffer_gettypename({}, {:x}, '{}')", fmt::ptr(self), idx,
                  tname);
    return api::from_c(self)->get_type(idx, tname);
}

tl::optional<typing::Value> buffer_collecttype(const RDBuffer* self, usize idx,
                                               const RDType* t) {
    spdlog::trace("buffer_collecttype({}, {:x}, {})", fmt::ptr(self), idx,
                  fmt::ptr(t));

    if(!t) return tl::nullopt;

    auto v = api::from_c(self)->get_type(idx, *t);

    v.map([idx](const typing::Value& v) {
        state::context->collectedtypes.emplace_back(idx, v.type);
    });

    return v;
}
tl::optional<typing::Value> buffer_collecttypename(const RDBuffer* self,
                                                   usize idx,
                                                   std::string_view tname) {
    spdlog::trace("buffer_collecttypename({}, {:x}, '{}')", fmt::ptr(self), idx,
                  tname);
    auto v = api::from_c(self)->get_type(idx, tname);

    v.map([idx](const typing::Value& v) {
        state::context->collectedtypes.emplace_back(idx, v.type);
    });

    return v;
}

} // namespace redasm::api::internal
