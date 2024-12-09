#pragma once

#include "mips_format.h"

namespace mips_macrodecoder {

void check_macro(RDAddress address, MIPSDecodedInstruction& dec, bool big);

} // namespace mips_macrodecoder
