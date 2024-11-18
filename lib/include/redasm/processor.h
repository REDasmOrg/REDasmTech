#pragma once

#include <redasm/common.h>
#include <redasm/rdil.h>
#include <redasm/renderer.h>

RD_HANDLE(RDEmulator);

typedef enum RDRefType {
    REF_UNKNOWN = 0,

    REF_READ = (1 << 0),
    REF_WRITE = (1 << 1),

    // Code specific
    REF_CALL = (1 << 2),
    REF_JUMP = (1 << 3),

    REF_INDIRECT = (1 << 4),
} RDRefType;

typedef enum RDInstructionType {
    IT_NONE = 0,
    IT_STOP = (1 << 0),
    IT_JUMP = (1 << 1),
    IT_CALL = (1 << 2),

} RDInstructionType;

typedef struct RDRef {
    RDAddress address;
    usize type;
} RDRef;

struct RDInstructionDetail {
    RDAddress address;
    usize size;
    usize type;
};

REDASM_EXPORT void rdemulator_addref(RDEmulator* self, RDAddress address,
                                     usize type);
REDASM_EXPORT void rdemulator_settype(RDEmulator* self, RDAddress address,
                                      const char* tname);

struct RDProcessor;

typedef void (*RDProcessorEmulate)(const RDProcessor*, RDEmulator*,
                                   RDInstructionDetail*);
typedef bool (*RDProcessorLift)(const RDProcessor*, RDAddress, RDILList*);
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
    RDProcessorLift lift;
    RDProcessorRenderSegment rendersegment;
    RDProcessorRenderFunction renderfunction;
    RDProcessorRenderInstruction renderinstruction;
    RDProcessorFree free;
} RDProcessor;

REDASM_EXPORT usize rd_getprocessors(const RDProcessor** processors);
REDASM_EXPORT void rd_registerprocessor(const RDProcessor* proc);
REDASM_EXPORT void rd_setprocessor(const char* name);
