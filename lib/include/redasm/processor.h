#pragma once

#include "common.h"
#include "renderer.h"

RD_HANDLE(RDEmulator);

typedef enum RDRefType {
    REF_UNKNOWN = 0,

    DR_ADDRESS,
    DR_READ,
    DR_WRITE,

    CR_CALL,
    CR_JUMP,
    CR_FLOW,
} RDRefType;

struct RDRef {
    RDAddress address;
    usize type;
};

REDASM_EXPORT void rdemulator_addcoderef(RDEmulator* self, RDAddress address,
                                         usize cr);
REDASM_EXPORT void rdemulator_adddataref(RDEmulator* self, RDAddress address,
                                         usize dr);
REDASM_EXPORT void rdemulator_settype(RDEmulator* self, RDAddress address,
                                      const char* tname);

struct RDProcessor;

typedef usize (*RDProcessorEmulate)(const RDProcessor*, RDAddress, RDEmulator*);
typedef bool (*RDProcessorRenderSegment)(const RDProcessor*,
                                         const RDRendererParams*);
typedef bool (*RDProcessorRenderFunction)(const RDProcessor*,
                                          const RDRendererParams*);
typedef bool (*RDProcessorRenderInstruction)(const RDProcessor*,
                                             const RDRendererParams*);
typedef void (*RDProcessorFree)(RDProcessor*);

typedef struct RDProcessor {
    const char* name;
    void* userdata;

    RDProcessorEmulate emulate;
    RDProcessorRenderSegment rendersegment;
    RDProcessorRenderFunction renderfunction;
    RDProcessorRenderInstruction renderinstruction;
    RDProcessorFree free;
} RDProcessor;

REDASM_EXPORT usize rd_getprocessors(const RDProcessor** processors);
REDASM_EXPORT void rd_registerprocessor(const RDProcessor* proc);
REDASM_EXPORT void rd_setprocessor(const char* name);
