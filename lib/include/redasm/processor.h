#pragma once

#include "common.h"
#include "renderer.h"

typedef struct RDEmulateResult {
    RDAddress address;
    usize delayslot;
    bool canflow;
} RDEmulateResult;

struct RDProcessor;

typedef usize (*RDProcessorEmulate)(const RDProcessor*, RDEmulateResult*);
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
REDASM_EXPORT void rd_selectprocessor(const char* name);
