#include "../utils/utils.h"
#include "../context.h"
#include "../state.h"
#include <redasm/utils.h>

namespace {

template<typename T>
T rol_impl(T val, T amt) {
    static constexpr T BITS_COUNT = sizeof(T) * CHAR_BIT;
    if(!amt) return val;
    return (val << amt) | (val >> (BITS_COUNT - amt));
}

template<typename T>
T ror_impl(T val, T amt) {
    static constexpr T BITS_COUNT = sizeof(T) * CHAR_BIT;
    if(!amt) return val;
    return (val >> amt) | (val << (BITS_COUNT - amt));
}

} // namespace

const char* rd_tohex_n(usize val, usize n) {
    spdlog::trace("rd_tohex_n({}, {})", val, n);
    static std::string res;
    if(redasm::state::context) res = redasm::state::context->to_hex(val, n);
    return res.empty() ? nullptr : res.c_str();
}

const char* rd_tohex(usize val) {
    spdlog::trace("rd_tohex({})", val);
    static std::string res;
    if(redasm::state::context) res = redasm::state::context->to_hex(val);
    return res.empty() ? nullptr : res.c_str();
}

usize rd_countbits(i64 val) {
    spdlog::trace("rd_countbits({:x})", val);
    return redasm::utils::count_bits(val);
}

u16 rd_rol16(u16 val, u16 amt) {
    spdlog::trace("rd_rol16({}, {})", val, amt);
    return ror_impl(val, amt);
}

u32 rd_rol32(u32 val, u32 amt) {
    spdlog::trace("rd_rol32({}, {})", val, amt);
    return ror_impl(val, amt);
}

u64 rd_rol64(u64 val, u64 amt) {
    spdlog::trace("rd_rol64({}, {})", val, amt);
    return ror_impl(val, amt);
}

u16 rd_ror16(u16 val, u16 amt) {
    spdlog::trace("rd_rol16({}, {})", val, amt);
    return ror_impl(val, amt);
}

u32 rd_ror32(u32 val, u32 amt) {
    spdlog::trace("rd_rol32({}, {})", val, amt);
    return ror_impl(val, amt);
}

u64 rd_ror64(u64 val, u64 amt) {
    spdlog::trace("rd_rol64({}, {})", val, amt);
    return ror_impl(val, amt);
}
