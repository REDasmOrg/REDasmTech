#pragma once

#include <redasm/common.h>
#include <redasm/function.h>
#include <redasm/instruction.h>
#include <redasm/rdil.h>
#include <redasm/renderer.h>
#include <redasm/segment.h>

RD_HANDLE(RDEmulator);

typedef enum RDRefType {
    DR_READ = 1,
    DR_WRITE,
    DR_ADDRESS,

    CR_CALL,
    CR_JUMP,
} RDRefType;

typedef struct RDRef {
    RDAddress address;
    usize type;
} RDRef;

// struct RDProcessorState;
struct RDProcessor;

// clang-format off
typedef void (*RDProcessorSetup)(const RDProcessor*, RDEmulator*);
typedef const char* (*RDProcessorGetRegisterName)(const RDProcessor*, int);
typedef void (*RDProcessorDecode)(const RDProcessor*, RDInstruction*);
typedef void (*RDProcessorEmulate)(const RDProcessor*, RDEmulator*, const RDInstruction*);
typedef bool (*RDProcessorLift)(const RDProcessor*, RDILList*, const RDInstruction*);
typedef void (*RDProcessorRenderSegment)(const RDProcessor*, RDRenderer*, const RDSegment*);
typedef void (*RDProcessorRenderFunction)(const RDProcessor*, RDRenderer*, const RDFunction*);
typedef void (*RDProcessorRenderInstruction)(const RDProcessor*, RDRenderer*, const RDInstruction*);
typedef void (*RDProcessorFree)(RDProcessor*);
// clang-format on

typedef struct RDProcessor {
    const char* id;
    const char* name;

    void* userdata;
    int address_size;
    int integer_size;

    RDProcessorGetRegisterName getregistername;
    RDProcessorSetup setup;
    RDProcessorDecode decode;
    RDProcessorEmulate emulate;
    RDProcessorLift lift;
    RDProcessorRenderSegment rendersegment;
    RDProcessorRenderFunction renderfunction;
    RDProcessorRenderInstruction renderinstruction;
    RDProcessorFree free;
} RDProcessor;

REDASM_EXPORT void rdemulator_flow(RDEmulator* self, RDAddress flowaddr);
REDASM_EXPORT u64 rdemulator_getreg(const RDEmulator* self, int regid);
REDASM_EXPORT void rdemulator_setreg(RDEmulator* self, int regid, u64 val);
REDASM_EXPORT u64 rdemulator_updreg(RDEmulator* self, int regid, u64 val,
                                    u64 mask);
REDASM_EXPORT u64 rdemulator_getstate(const RDEmulator* self,
                                      const char* state);
REDASM_EXPORT u64 rdemulator_takestate(RDEmulator* self, const char* state);
REDASM_EXPORT void rdemulator_delstate(RDEmulator* self, const char* state);
REDASM_EXPORT void rdemulator_setstate(RDEmulator* self, const char* state,
                                       u64 val);
REDASM_EXPORT u64 rdemulator_updstate(RDEmulator* self, const char* state,
                                      u64 val, u64 mask);
REDASM_EXPORT void rdemulator_addref(RDEmulator* self, RDAddress toaddr,
                                     usize type);

REDASM_EXPORT const RDProcessor* rd_getprocessor(void);
REDASM_EXPORT usize rd_getprocessors(const RDProcessor** processors);
REDASM_EXPORT void rd_registerprocessor(const RDProcessor* proc);
REDASM_EXPORT void rd_setprocessor(const char* name);
