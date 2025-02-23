#pragma once

#include <redasm/common.h>
#include <redasm/processor.h>

// clang-format off
typedef void (*RDEnvironmentUpdateInstruction)(RDInstruction*);
typedef const char* (*RDEnvironmentGetOperandSymbol)(const RDInstruction*, usize idx);
// clang-format on

typedef struct RDEnvironment {
    const char* name;
    RDEnvironmentUpdateInstruction update_instruction;
    RDEnvironmentGetOperandSymbol get_operand_symbol;
} RDEnvironment;

REDASM_EXPORT bool rdenvironment_init(RDEnvironment* self, const char* name);
REDASM_EXPORT const RDEnvironment* rd_getenvironment(void);
