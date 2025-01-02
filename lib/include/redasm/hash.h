#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

REDASM_EXPORT u32 rd_adler32(const void* data, usize size);
