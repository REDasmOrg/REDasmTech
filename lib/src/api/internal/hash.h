#pragma once

#include <redasm/analyzer.h>

namespace redasm::api::internal {

u32 adler32(const void* data, usize size);

}
