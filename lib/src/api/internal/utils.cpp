#include "utils.h"
#include "../../context.h"
#include "../../memory/byteorder.h"
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

u16 from_le16(u16 hostval) { return byteorder::from_littleendian(hostval); }
u32 from_le32(u32 hostval) { return byteorder::from_littleendian(hostval); }
u64 from_le64(u64 hostval) { return byteorder::from_littleendian(hostval); }
u16 from_be16(u16 hostval) { return byteorder::from_bigendian(hostval); }
u32 from_be32(u32 hostval) { return byteorder::from_bigendian(hostval); }
u64 from_be64(u64 hostval) { return byteorder::from_bigendian(hostval); }

u16 to_le16(u16 hostval) { return byteorder::to_littleendian(hostval); }
u32 to_le32(u32 hostval) { return byteorder::to_littleendian(hostval); }
u64 to_le64(u64 hostval) { return byteorder::to_littleendian(hostval); }
u16 to_be16(u16 hostval) { return byteorder::to_bigendian(hostval); }
u32 to_be32(u32 hostval) { return byteorder::to_bigendian(hostval); }
u64 to_be64(u64 hostval) { return byteorder::to_bigendian(hostval); }

} // namespace redasm::api::internal
