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
// typedef void* (*RDProcessorStateInit)(const RDProcessor*);
// typedef void (*RDProcessorStateFini)(const RDProcessor*, void*);
// typedef void* (*RDProcessorStateCopy)(const RDProcessor*, const void*);

typedef const char* (*RDProcessorGetRegisterName)(const RDProcessor*, int);
typedef void (*RDProcessorDecode)(const RDProcessor*, RDInstruction*);
typedef void (*RDProcessorEmulate)(const RDProcessor*, RDEmulator*, const RDInstruction*);
typedef bool (*RDProcessorLift)(const RDProcessor*, RDILList*, const RDInstruction*);
typedef void (*RDProcessorRenderSegment)(const RDProcessor*, RDRenderer*, const RDSegment*);
typedef void (*RDProcessorRenderFunction)(const RDProcessor*, RDRenderer*, const RDFunction*);
typedef void (*RDProcessorRenderInstruction)(const RDProcessor*, RDRenderer*, const RDInstruction*);
typedef void (*RDProcessorFree)(RDProcessor*);
// clang-format on

// typedef struct RDProcessorState {
//     RDProcessorStateInit init;
//     RDProcessorStateFini fini;
//     RDProcessorStateCopy copy;
// } RDProcessorState;

typedef struct RDProcessor {
    const char* id;
    const char* name;

    void* userdata;
    TypeId address_type;
    TypeId integer_type;
    // RDProcessorState state;

    RDProcessorGetRegisterName getregistername;
    RDProcessorDecode decode;
    RDProcessorEmulate emulate;
    RDProcessorLift lift;
    RDProcessorRenderSegment rendersegment;
    RDProcessorRenderFunction renderfunction;
    RDProcessorRenderInstruction renderinstruction;
    RDProcessorFree free;
} RDProcessor;

REDASM_EXPORT void rdemulator_addref(RDEmulator* e, RDAddress toaddr,
                                     usize type);
REDASM_EXPORT void rdemulator_settype(RDEmulator* e, RDAddress addr,
                                      const RDType* type);
REDASM_EXPORT usize rd_getprocessors(const RDProcessor** processors);
REDASM_EXPORT void rd_registerprocessor(const RDProcessor* proc);
REDASM_EXPORT void rd_setprocessor(const char* name);
