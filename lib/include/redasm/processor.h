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

    CR_FLOW,
    CR_CALL,
    CR_JUMP,
} RDRefType;

typedef struct RDRef {
    RDAddress address;
    usize type;
} RDRef;

struct RDProcessor;

// clang-format off
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

REDASM_EXPORT usize rd_getprocessors(const RDProcessor** processors);
REDASM_EXPORT void rd_registerprocessor(const RDProcessor* proc);
REDASM_EXPORT void rd_setprocessor(const char* name);
