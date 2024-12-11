#pragma once

#include "mips_format.h"
#include <redasm/redasm.h>
#include <string_view>
#include <type_traits>

namespace mips_decoder {

template<typename T>
std::make_signed_t<T> sign_ext(T val, int valbits) {
    T m = 1;
    m <<= valbits - 1;
    return static_cast<std::make_signed_t<T>>((val ^ m) - m);
}

const char* mnemonic(usize id);
const char* reg(u32 r);
std::string_view cop0_reg(u32 r);
std::string_view copn_reg(u32 r);
bool decode(RDAddress address, MIPSDecodedInstruction& dec, bool big,
            bool one = false);

} // namespace mips_decoder
