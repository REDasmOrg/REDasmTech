#include "../internal/utils.h"
#include <redasm/utils.h>

const char* rd_tohex_n(usize val, usize n) {
    static std::string res;
    res = redasm::api::internal::to_hex_n(val, n);
    return res.empty() ? nullptr : res.c_str();
}

const char* rd_tohex(usize val) {
    static std::string res;
    res = redasm::api::internal::to_hex(val);
    return res.empty() ? nullptr : res.c_str();
}

u16 rd_rol16(u16 val, u16 amt) {
    return redasm::api::internal::rol16(val, amt);
}

u32 rd_rol32(u32 val, u32 amt) {
    return redasm::api::internal::rol32(val, amt);
}

u64 rd_rol64(u64 val, u64 amt) {
    return redasm::api::internal::rol64(val, amt);
}

u16 rd_ror16(u16 val, u16 amt) {
    return redasm::api::internal::ror16(val, amt);
}

u32 rd_ror32(u32 val, u32 amt) {
    return redasm::api::internal::ror32(val, amt);
}

u64 rd_ror64(u64 val, u64 amt) {
    return redasm::api::internal::ror64(val, amt);
}

u16 rd_fromle16(u16 hostval) {
    return redasm::api::internal::from_le16(hostval);
}

u32 rd_fromle32(u32 hostval) {
    return redasm::api::internal::from_le32(hostval);
}

u64 rd_fromle64(u64 hostval) {
    return redasm::api::internal::from_le64(hostval);
}

u16 rd_frombe16(u16 hostval) {
    return redasm::api::internal::from_be16(hostval);
}

u32 rd_frombe32(u32 hostval) {
    return redasm::api::internal::from_be32(hostval);
}

u64 rd_frombe64(u64 hostval) {
    return redasm::api::internal::from_be64(hostval);
}

u16 rd_tole16(u16 hostval) { return redasm::api::internal::to_le16(hostval); }
u32 rd_tole32(u32 hostval) { return redasm::api::internal::to_le32(hostval); }
u64 rd_tole64(u64 hostval) { return redasm::api::internal::to_le64(hostval); }
u16 rd_tobe16(u16 hostval) { return redasm::api::internal::to_be16(hostval); }
u32 rd_tobe2(u32 hostval) { return redasm::api::internal::to_be32(hostval); }
u64 rd_tobe64(u64 hostval) { return redasm::api::internal::to_be64(hostval); }
