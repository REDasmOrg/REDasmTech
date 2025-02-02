#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

REDASM_EXPORT const char* rd_tohex_n(usize val, usize n);
REDASM_EXPORT const char* rd_tohex(usize val);
REDASM_EXPORT usize rd_countbits(i64 val);
REDASM_EXPORT u16 rd_rol16(u16 val, u16 amt);
REDASM_EXPORT u32 rd_rol32(u32 val, u32 amt);
REDASM_EXPORT u64 rd_rol64(u64 val, u64 amt);
REDASM_EXPORT u16 rd_ror16(u16 val, u16 amt);
REDASM_EXPORT u32 rd_ror32(u32 val, u32 amt);
REDASM_EXPORT u64 rd_ror64(u64 val, u64 amt);
