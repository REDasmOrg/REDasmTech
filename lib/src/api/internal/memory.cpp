#include "memory.h"
#include "../../context.h"
#include "../../memory/stringfinder.h"
#include "../../state.h"
#include "../marshal.h"
#include "buffer.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

bool is_address(RDAddress address) {
    spdlog::trace("is_address({:x})", address);
    const Context* ctx = state::context;
    return ctx && ctx->is_address(address);
}

bool check_string(RDAddress address, RDStringResult* mi) {
    spdlog::trace("check_string({:x}, {})", address, fmt::ptr(mi));

    if(!mi) return false;

    auto idx = state::context->address_to_index(address);

    if(!idx) {
        *mi = {};
        return false;
    }

    auto res = stringfinder::classify(*idx);
    if(res) *mi = *res;
    return res.has_value();
}

bool memory_map(RDAddress startaddr, RDAddress endaddr) {
    spdlog::trace("memory_map({:x}, {:x})", startaddr, endaddr);

    if(startaddr > endaddr) // NOLINT
        endaddr = startaddr;

    return state::context->memory_map(startaddr, endaddr - startaddr);
}

bool memory_map_n(RDAddress base, usize size) {
    spdlog::trace("memory_map_n({:x}, {:x})", base, size);
    return state::context->memory_map(base, size);
}

void memory_copy(RDAddress address, RDOffset start, RDOffset end) {
    spdlog::trace("memory_copy({:x}, {:x}, {:x})", address, start, end);
    state::context->memory_copy(address, start, end);
}

void memory_copy_n(RDAddress address, RDOffset start, usize size) {
    spdlog::trace("memory_copy_n({:x}, {:x}, {:x})", address, start, size);
    state::context->memory_copy_n(address, start, size);
}

usize memory_bytes(const Byte** b) {
    spdlog::trace("memory_bytes({})", fmt::ptr(b));

    if(b) *b = state::context->memory->data();

    return state::context->memory->size();
}

void memory_info(RDMemoryInfo* mi) {
    spdlog::trace("memory_info({})", fmt::ptr(mi));

    if(!mi) return;

    *mi = {
        {state::context->baseaddress},
        {state::context->end_baseaddress()},
        state::context->memory->size(),
    };
}

usize read(RDAddress address, void* dst, usize n) {
    spdlog::trace("read({}, {}, {})", address, fmt::ptr(dst), n);

    if(!state::context) return false;

    auto idx = state::context->address_to_index(address);
    if(idx) return state::context->memory->read(*idx, dst, n);
    return n;
}

usize memory_size() {
    spdlog::trace("memory_size()");
    return state::context->memory->size();
}

tl::optional<bool> get_bool(RDAddress address) {
    spdlog::trace("get_bool({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getbool(api::to_c(state::context->memory.get()),
                                        *idx);

    return tl::nullopt;
}

tl::optional<char> get_char(RDAddress address) {
    spdlog::trace("get_char({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getchar(api::to_c(state::context->memory.get()),
                                        *idx);

    return tl::nullopt;
}

tl::optional<u8> get_u8(RDAddress address) {
    spdlog::trace("get_u8({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getu8(api::to_c(state::context->memory.get()),
                                      *idx);

    return tl::nullopt;
}

tl::optional<u16> get_u16(RDAddress address) {
    spdlog::trace("get_u16({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getu16(api::to_c(state::context->memory.get()),
                                       *idx);

    return tl::nullopt;
}

tl::optional<u32> get_u32(RDAddress address) {
    spdlog::trace("get_u32({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getu32(api::to_c(state::context->memory.get()),
                                       *idx);

    return tl::nullopt;
}

tl::optional<u64> get_u64(RDAddress address) {
    spdlog::trace("get_u64({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getu64(api::to_c(state::context->memory.get()),
                                       *idx);

    return tl::nullopt;
}

tl::optional<i8> get_i8(RDAddress address) {
    spdlog::trace("get_i8({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_geti8(api::to_c(state::context->memory.get()),
                                      *idx);

    return tl::nullopt;
}

tl::optional<i16> get_i16(RDAddress address) {
    spdlog::trace("get_i16({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_geti16(api::to_c(state::context->memory.get()),
                                       *idx);

    return tl::nullopt;
}

tl::optional<i32> get_i32(RDAddress address) {
    spdlog::trace("get_i32({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_geti32(api::to_c(state::context->memory.get()),
                                       *idx);

    return tl::nullopt;
}

tl::optional<i64> get_i64(RDAddress address) {
    spdlog::trace("get_i64({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_geti64(api::to_c(state::context->memory.get()),
                                       *idx);

    return tl::nullopt;
}

tl::optional<u16> get_u16be(RDAddress address) {
    spdlog::trace("get_u16be({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getu16be(
            api::to_c(state::context->memory.get()), *idx);

    return tl::nullopt;
}

tl::optional<u32> get_u32be(RDAddress address) {
    spdlog::trace("get_u32be({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getu32be(
            api::to_c(state::context->memory.get()), *idx);

    return tl::nullopt;
}

tl::optional<u64> get_u64be(RDAddress address) {
    spdlog::trace("get_u64be({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getu64be(
            api::to_c(state::context->memory.get()), *idx);

    return tl::nullopt;
}

tl::optional<i16> get_i16be(RDAddress address) {
    spdlog::trace("get_i16be({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_geti16be(
            api::to_c(state::context->memory.get()), *idx);

    return tl::nullopt;
}

tl::optional<i32> get_i32be(RDAddress address) {
    spdlog::trace("get_i32be({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_geti32be(
            api::to_c(state::context->memory.get()), *idx);

    return tl::nullopt;
}

tl::optional<i64> get_i64be(RDAddress address) {
    spdlog::trace("get_i64be({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_geti64be(
            api::to_c(state::context->memory.get()), *idx);

    return tl::nullopt;
}

tl::optional<std::string> get_strz(RDAddress address) {
    spdlog::trace("get_strz({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getstrz(api::to_c(state::context->memory.get()),
                                        *idx);

    return tl::nullopt;
}

tl::optional<std::string> get_str(RDAddress address, usize n) {
    spdlog::trace("get_str({:x}, {})", address, n);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getstr(api::to_c(state::context->memory.get()),
                                       *idx, n);

    return tl::nullopt;
}

tl::optional<std::string> get_wstrz(RDAddress address) {
    spdlog::trace("get_wstrz({:x})", address);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getwstrz(
            api::to_c(state::context->memory.get()), *idx);

    return tl::nullopt;
}

tl::optional<std::string> get_wstr(RDAddress address, usize n) {
    spdlog::trace("get_wstr({:x}, {})", address, n);
    if(auto idx = state::context->address_to_index(address); idx)
        return internal::buffer_getwstr(api::to_c(state::context->memory.get()),
                                        *idx, n);

    return tl::nullopt;
}

} // namespace redasm::api::internal
