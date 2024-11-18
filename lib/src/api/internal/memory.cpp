#include "memory.h"
#include "../../context.h"
#include "../../memory/stringfinder.h"
#include "../../state.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

void memory_map(RDAddress base, usize size) {
    spdlog::trace("memory_map({:x}, {:x})", base, size);
    state::context->memory_map(base, size);
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

    if(b)
        *b = state::context->memory->data();

    return state::context->memory->size();
}

bool check_string(RDAddress address, RDStringResult* mi) {
    spdlog::trace("check_string({:x}, {})", address, fmt::ptr(mi));

    if(!mi)
        return false;

    auto idx = state::context->address_to_index(address);

    if(!idx) {
        *mi = {};
        return false;
    }

    auto res = stringfinder::classify(*idx);
    if(res)
        *mi = *res;
    return res.has_value();
}

void memory_info(RDMemoryInfo* mi) {
    spdlog::trace("memory_info({})", fmt::ptr(mi));

    if(!mi)
        return;

    *mi = {
        {state::context->baseaddress},
        {state::context->end_baseaddress()},
        state::context->memory->size(),
        state::context->bits.value_or(0),
    };
}

tl::optional<typing::Value> map_type(RDAddress address,
                                     std::string_view tname) {
    spdlog::trace("map_type({}, '{}')", address, tname);

    auto idx = state::context->address_to_index(address);
    if(!idx)
        return tl::nullopt;

    usize sz = state::context->types.size_of(tname);
    state::context->memory_copy(*idx, *idx, *idx + sz);

    if(!state::context->set_type(*idx, tname))
        return tl::nullopt;

    return state::context->memory->get_type(*idx, tname);
}

usize memory_read(RDAddress address, char* data, usize n) {
    spdlog::trace("memory_read({}, {}, {})", address, fmt::ptr(data), n);

    auto idx = state::context->address_to_index(address);
    if(!idx)
        return 0;

    usize c = 0;

    for(usize i = *idx; i < state::context->memory->size(); i++) {
        Byte b = state::context->memory->at(i);

        if(!b.has_byte() || c == n)
            break;

        if(data) {
            char ch = static_cast<char>(b.byte());
            data[c++] = ch; // static_cast<char>(b.byte());
        }
        else
            c++;
    }

    return c;
}

usize memory_size() {
    spdlog::trace("memory_size()");
    return state::context->memory->size();
}

bool memory_setflags(RDAddress address, u32 flags) {
    spdlog::trace("memory_setflags({:x}, {:x})", address, flags);

    Context* ctx = state::context;

    return ctx->address_to_index(address).map_or(
        [&](MIndex idx) {
            ctx->memory->at(idx).set(flags);
            return true;
        },
        false);
}

} // namespace redasm::api::internal
