#include "../internal/byteorder.h"
#include <redasm/byteorder.h>

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
