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
