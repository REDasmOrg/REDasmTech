#pragma once

#include <redasm/redasm.h>

namespace redasm::api::internal {

u16 from_le16(u16 hostval);
u32 from_le32(u32 hostval);
u64 from_le64(u64 hostval);
u16 from_be16(u16 hostval);
u32 from_be32(u32 hostval);
u64 from_be64(u64 hostval);
u16 to_le16(u16 hostval);
u32 to_le32(u32 hostval);
u64 to_le64(u64 hostval);
u16 to_be16(u16 hostval);
u32 to_be32(u32 hostval);
u64 to_be64(u64 hostval);

} // namespace redasm::api::internal
