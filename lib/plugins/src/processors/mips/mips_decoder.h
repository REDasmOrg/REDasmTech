#pragma once

#include "mips_format.h"
#include <redasm/redasm.h>
#include <string_view>
#include <type_traits>

namespace mips_decoder {

inline i32 signext_16_32(i16 v) { return static_cast<i32>(v); }

const char* mnemonic(usize id);
const char* reg(u32 r);
std::string_view cop0_reg(u32 r);
std::string_view copn_reg(u32 r);
bool decode(RDAddress address, MIPSDecodedInstruction& dec, bool big,
            bool one = false);

} // namespace mips_decoder
