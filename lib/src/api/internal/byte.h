#pragma once

#include <redasm/byte.h>

namespace redasm::api::internal {

bool byte_isunknown(RDByte self);
bool byte_iscode(RDByte self);
bool byte_isdata(RDByte self);
bool byte_has(RDByte self, usize f);
bool byte_hasbyte(RDByte self);
bool byte_getbyte(RDByte self, u8* b);

} // namespace redasm::api::internal
