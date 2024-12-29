#include "utils.h"
#include "../../context.h"
#include "../../state.h"

namespace redasm::api::internal {

namespace {

template<typename T>
T rol_impl(T val, T amt) {
    static constexpr T BITS_COUNT = sizeof(T) * CHAR_BIT;
    if(!amt)
        return val;
    return (val << amt) | (val >> (BITS_COUNT - amt));
}

template<typename T>
T ror_impl(T val, T amt) {
    static constexpr T BITS_COUNT = sizeof(T) * CHAR_BIT;
    if(!amt)
        return val;
    return (val >> amt) | (val << (BITS_COUNT - amt));
}

} // namespace

std::string to_hex_n(usize v, int n) {
    if(state::context)
        return state::context->to_hex(v, n);
    return {};
}

std::string to_hex(usize v) {
    if(state::context)
        return state::context->to_hex(v);
    return {};
}

u16 rol16(u16 val, u16 amt) { return rol_impl(val, amt); }
u32 rol32(u32 val, u32 amt) { return rol_impl(val, amt); }
u64 rol64(u64 val, u64 amt) { return rol_impl(val, amt); }
u16 ror16(u16 val, u16 amt) { return ror_impl(val, amt); }
u32 ror32(u32 val, u32 amt) { return ror_impl(val, amt); }
u64 ror64(u64 val, u64 amt) { return ror_impl(val, amt); }

} // namespace redasm::api::internal
