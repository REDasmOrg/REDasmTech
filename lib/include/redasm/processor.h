#pragma once

#include "common.h"
#include "renderer.h"

RD_HANDLE(RDEmulateResult);

REDASM_EXPORT RDAddress rdemulateresult_getaddress(const RDEmulateResult* self);
REDASM_EXPORT void rdemulateresult_setsize(RDEmulateResult* self, usize s);
REDASM_EXPORT void rdemulateresult_addreturn(RDEmulateResult* self);
REDASM_EXPORT void rdemulateresult_addcall(RDEmulateResult* self,
                                           RDAddress address);
REDASM_EXPORT void rdemulateresult_addcallunresolved(RDEmulateResult* self);
REDASM_EXPORT void rdemulateresult_addbranch(RDEmulateResult* self,
                                             RDAddress address);
REDASM_EXPORT void rdemulateresult_addbranchtrue(RDEmulateResult* self,
                                                 RDAddress address);
REDASM_EXPORT void rdemulateresult_addbranchfalse(RDEmulateResult* self,
                                                  RDAddress address);
REDASM_EXPORT void rdemulateresult_addbranchunresolved(RDEmulateResult* self);

struct RDProcessor;

typedef void (*RDProcessorEmulate)(const RDProcessor*, RDEmulateResult*);
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
