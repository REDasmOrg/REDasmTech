#pragma once

#include <redasm/redasm.h>
#include <string>

namespace redasm::api::internal {

std::string to_hex_n(usize v, int n);
std::string to_hex(usize v);
u16 rol16(u16 val, u16 amt);
u32 rol32(u32 val, u32 amt);
u64 rol64(u64 val, u64 amt);
u16 ror16(u16 val, u16 amt);
u32 ror32(u32 val, u32 amt);
u64 ror64(u64 val, u64 amt);

} // namespace redasm::api::internal
