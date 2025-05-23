#include "x86_cc.h"
#include <Zydis/Zydis.h>

namespace x86_cc {

static const int EAX_ONLY[] = {ZYDIS_REGISTER_EAX, -1};

static const int CDECL32_CALLEE[] = {
    ZYDIS_REGISTER_EAX,
    ZYDIS_REGISTER_ECX,
    ZYDIS_REGISTER_EDX,
    -1,
};

RDCallingConvention cdecl_cc = {
    .name = "x86_32/cdecl",
    .features = CC_USE_FP | CC_VARIADIC,

    .arg_registers = nullptr, // Arguments are pushed on stack
    .ret_registers = EAX_ONLY,
    .callee_saved = CDECL32_CALLEE,

    .stack_cleanup = SC_CALLER,
    .stack_direction = SD_DOWNWARDS,
};

} // namespace x86_cc
