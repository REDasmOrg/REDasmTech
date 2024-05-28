#include "stream.h"
#include "../../context.h"
#include "../../memory/memorystream.h"
#include "../../memory/stream.h"
#include "../../state.h"
#include "../marshal.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

RDStream* stream_createfromfile() { return api::to_c(new Stream()); }
RDStream* stream_createfrommemory() { return api::to_c(new MemoryStream()); }

usize stream_seek(RDStream* self, usize off) {
    return api::from_c(self)->seek(off);
}

usize stream_move(RDStream* self, isize off) {
    return api::from_c(self)->move(off);
}

void stream_rewind(RDStream* self) { api::from_c(self)->rewind(); }

usize stream_getposition(const RDStream* self) {
    return api::from_c(self)->position;
}

tl::optional<typing::Value> stream_peek_type(const RDStream* self,
                                             std::string_view tname) {
    return api::from_c(self)->peek_type(tname);
}

tl::optional<std::string> stream_peek_stringz(const RDStream* self) {
    return api::from_c(self)->peek_string();
}

tl::optional<std::string> stream_peek_string(const RDStream* self, usize n) {
    return api::from_c(self)->peek_string(n);
}

tl::optional<u8> stream_peek_u8(const RDStream* self) {
    return api::from_c(self)->peek_u8();
}

tl::optional<u16> stream_peek_u16(const RDStream* self) {
    return api::from_c(self)->peek_u16(false);
}

tl::optional<u32> stream_peek_u32(const RDStream* self) {
    return api::from_c(self)->peek_u32(false);
}

tl::optional<u64> stream_peek_u64(const RDStream* self) {
    return api::from_c(self)->peek_u64(false);
}

tl::optional<i8> stream_peek_i8(const RDStream* self) {
    return api::from_c(self)->peek_i8();
}

tl::optional<i16> stream_peek_i16(const RDStream* self) {
    return api::from_c(self)->peek_i16(false);
}

tl::optional<i32> stream_peek_i32(const RDStream* self) {
    return api::from_c(self)->peek_i32(false);
}

tl::optional<i64> stream_peek_i64(const RDStream* self) {
    return api::from_c(self)->peek_i64(false);
}

tl::optional<u16> stream_peek_u16be(const RDStream* self) {
    return api::from_c(self)->peek_u16(true);
}

tl::optional<u32> stream_peek_u32be(const RDStream* self) {
    return api::from_c(self)->peek_u32(true);
}

tl::optional<u64> stream_peek_u64be(const RDStream* self) {
    return api::from_c(self)->peek_u64(true);
}

tl::optional<i16> stream_peek_i16be(const RDStream* self) {
    return api::from_c(self)->peek_i16(true);
}

tl::optional<i32> stream_peek_i32be(const RDStream* self) {
    return api::from_c(self)->peek_i32(true);
}

tl::optional<i64> stream_peek_i64be(const RDStream* self) {
    return api::from_c(self)->peek_i64(true);
}

tl::optional<typing::Value> stream_read_type(RDStream* self,
                                             std::string_view tname) {
    return api::from_c(self)->read_type(tname);
}

tl::optional<std::string> stream_read_stringz(RDStream* self) {
    return api::from_c(self)->read_string();
}

tl::optional<std::string> stream_read_string(RDStream* self, usize n) {
    return api::from_c(self)->read_string(n);
}

tl::optional<u8> stream_read_u8(RDStream* self) {
    return api::from_c(self)->read_u8();
}

tl::optional<u16> stream_read_u16(RDStream* self) {
    return api::from_c(self)->read_u16(false);
}

tl::optional<u32> stream_read_u32(RDStream* self) {
    return api::from_c(self)->read_u32(false);
}

tl::optional<u64> stream_read_u64(RDStream* self) {
    return api::from_c(self)->read_u64(false);
}

tl::optional<i8> stream_read_i8(RDStream* self) {
    return api::from_c(self)->read_i8();
}

tl::optional<i16> stream_read_i16(RDStream* self) {
    return api::from_c(self)->read_i16(false);
}

tl::optional<i32> stream_read_i32(RDStream* self) {
    return api::from_c(self)->read_i32(false);
}

tl::optional<i64> stream_read_i64(RDStream* self) {
    return api::from_c(self)->read_i64(false);
}

tl::optional<u16> stream_read_u16be(RDStream* self) {
    return api::from_c(self)->read_u16(true);
}

tl::optional<u32> stream_read_u32be(RDStream* self) {
    return api::from_c(self)->read_u32(true);
}

tl::optional<u64> stream_read_u64be(RDStream* self) {
    return api::from_c(self)->read_u64(true);
}

tl::optional<i16> stream_read_i16be(RDStream* self) {
    return api::from_c(self)->read_i16(true);
}

tl::optional<i32> stream_read_i32be(RDStream* self) {
    return api::from_c(self)->read_i32(true);
}

tl::optional<i64> stream_read_i64be(RDStream* self) {
    return api::from_c(self)->read_i64(true);
}

tl::optional<typing::Value> stream_collect_type(RDStream* self,
                                                std::string_view tname) {
    AbstractStream* obj = api::from_c(self);
    usize pos = obj->position;
    auto t = obj->read_type(tname);

    if(t)
        state::context->collectedtypes.emplace_back(pos, tname);

    return t;
}

tl::optional<std::string> stream_collect_stringz(RDStream* self) {
    AbstractStream* obj = api::from_c(self);
    usize pos = obj->position;
    auto s = obj->read_string();

    if(s)
        state::context->collectedtypes.emplace_back(pos, "str");

    return s;
}

tl::optional<std::string> stream_collect_string(RDStream* self, usize n) {
    AbstractStream* obj = api::from_c(self);
    usize pos = obj->position;
    auto s = obj->read_string(n);

    if(s) {
        state::context->collectedtypes.emplace_back(
            pos, fmt::format("char[{}]", s->size()));
    }

    return s;
}

} // namespace redasm::api::internal
