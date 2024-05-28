#pragma once

#include "../../memory/byte.h"

namespace redasm::api::internal {

bool byte_isunknown(Byte self);
bool byte_iscode(Byte self);
bool byte_isdata(Byte self);
bool byte_has(Byte self, RDByteFlags f);
bool byte_hasbyte(Byte self);
bool byte_getbyte(Byte self, u8* b);

} // namespace redasm::api::internal
