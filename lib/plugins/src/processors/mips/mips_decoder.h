#pragma once

#include "mips_format.h"
#include <redasm/redasm.h>
#include <string_view>

namespace mips_decoder {

std::string_view reg(u32 r);
std::string_view cop0_reg(u32 r);
std::string_view copn_reg(u32 r);
bool decode(RDAddress address, MIPSDecodedInstruction& dec, bool big,
            bool one = false);

} // namespace mips_decoder
