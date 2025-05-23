#pragma once

#include <redasm/common.h>
#include <redasm/instruction.h>

// clang-format off
typedef void (*RDEnvironmentUpdateInstruction)(RDInstruction*);
typedef const char* (*RDEnvironmentGetOperandSymbol)(const RDInstruction*, usize idx);
// clang-format on

typedef enum RDStackCleanup {
    SC_NONE = 0,
    SC_CALLER,
    SC_CALLEE,
} RDStackCleanup;

typedef enum RDStackDirection {
    SD_DOWNWARDS = 0,
    SD_UPWARDS,
} RDStackDirection;

typedef enum RDCallingConventionFeatures {
    CC_USE_FP = 1 << 0,
    CC_VARIADIC = 1 << 1,
} RDCallingConventionFeatures;

typedef struct RDCallingConvention {
    const char* name;
    int features;

    const int* arg_registers;
    const int* ret_registers;
    const int* callee_saved;

    RDStackCleanup stack_cleanup;
    RDStackDirection stack_direction;
} RDCallingConvention;

typedef struct RDEnvironment {
    const char* name;
    RDEnvironmentUpdateInstruction update_instruction;
    RDEnvironmentGetOperandSymbol get_operand_symbol;
} RDEnvironment;

REDASM_EXPORT bool rdenvironment_init(RDEnvironment* self, const char* name);
REDASM_EXPORT const RDCallingConvention* rd_getcallingconvention(void);
REDASM_EXPORT const RDEnvironment* rd_getenvironment(void);
