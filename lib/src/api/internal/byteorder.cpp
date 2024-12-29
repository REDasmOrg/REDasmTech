#include "byteorder.h"
#include "../../memory/byteorder.h"

namespace redasm::api::internal {

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
