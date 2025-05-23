#pragma once

#include <redasm/common.h>
#include <redasm/environment.h>
#include <redasm/function.h>
#include <redasm/instruction.h>
#include <redasm/plugin.h>
#include <redasm/rdil.h>
#include <redasm/renderer.h>
#include <redasm/segment.h>
#include <redasm/sreg.h>

RD_HANDLE(RDEmulator);
RD_HANDLE(RDProcessor);

typedef enum RDRefType {
    DR_READ = 1,
    DR_WRITE,
    DR_ADDRESS,

    CR_CALL,
    CR_JUMP,
} RDRefType;

typedef enum RDProcessorFlags {
    PF_LITTLE = 0,
    PF_BIG = (1u << 1),
} RDProcessorFlags;

typedef struct RDRef {
    RDAddress address;
    usize type;
} RDRef;

// clang-format off
typedef void (*RDProcessorPluginSetup)(RDProcessor*, RDEmulator*);
typedef void (*RDProcessorPluginDecode)(RDProcessor*, RDInstruction*);
typedef void (*RDProcessorPluginEmulate)(RDProcessor*, RDEmulator*, const RDInstruction*);
typedef bool (*RDProcessorPluginLift)(RDProcessor*, RDILList*, const RDInstruction*);
typedef const char* (*RDProcessorPluginGetMnemonic)(const RDProcessor*, const RDInstruction*);
typedef const char* (*RDProcessorPluginGetRegisterName)(const RDProcessor*, int);
typedef const char** (*RDProcessorPluginGetPrologues)(const RDProcessor*);
typedef const int* (*RDProcessorPluginGetSegmentRegisters)(const RDProcessor*);
typedef const RDCallingConvention** (*RDProcessorPluginGetCallingConventions)(const RDProcessor*);
typedef RDAddress (*RDProcessorPluginNormalizeAddress)(const RDProcessor*, RDAddress);
typedef void (*RDProcessorPluginRenderSegment)(const RDProcessor*, RDRenderer*, const RDSegment*);
typedef void (*RDProcessorPluginRenderFunction)(const RDProcessor*, RDRenderer*, const RDFunction*);
typedef void (*RDProcessorPluginRenderInstruction)(const RDProcessor*, RDRenderer*, const RDInstruction*);
// clang-format on

typedef struct RDProcessorPlugin {
    RDPLUGIN_HEADER(RDProcessor)
    int address_size;
    int integer_size;
    RDProcessorPluginSetup setup;
    RDProcessorPluginGetMnemonic get_mnemonic;
    RDProcessorPluginGetRegisterName get_registername;
    RDProcessorPluginGetSegmentRegisters get_segmentregisters;
    RDProcessorPluginGetPrologues get_prologues;
    RDProcessorPluginGetCallingConventions get_callingconventions;
    RDProcessorPluginNormalizeAddress normalize_address;
    RDProcessorPluginDecode decode;
    RDProcessorPluginEmulate emulate;
    RDProcessorPluginLift lift;
    RDProcessorPluginRenderSegment render_segment;
    RDProcessorPluginRenderFunction render_function;
    RDProcessorPluginRenderInstruction render_instruction;
} RDProcessorPlugin;

define_slice(RDProcessorPluginSlice, const RDProcessorPlugin*);

REDASM_EXPORT const RDSegment* rdemulator_getsegment(const RDEmulator* self);
REDASM_EXPORT u32 rdemulator_getdslotinfo(const RDEmulator* self,
                                          const RDInstruction** dslot);
REDASM_EXPORT void rdemulator_flow(RDEmulator* self, RDAddress flowaddr);
REDASM_EXPORT RDRegValue rdemulator_getreg(const RDEmulator* self, int regid);
REDASM_EXPORT void rdemulator_unsetreg(RDEmulator* self, int regid);
REDASM_EXPORT void rdemulator_setreg(RDEmulator* self, int regid, u64 val);
REDASM_EXPORT RDRegValue rdemulator_updreg(RDEmulator* self, int regid, u64 val,
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

REDASM_EXPORT bool rd_registerprocessor(const RDProcessorPlugin* plugin);
REDASM_EXPORT bool rd_registerprocessor_ex(const RDProcessorPlugin* plugin,
                                           const char* origin);
REDASM_EXPORT const RDProcessorPluginSlice* rd_getprocessorplugins(void);
REDASM_EXPORT const RDProcessorPlugin* rd_getprocessorplugin(void);
REDASM_EXPORT const RDProcessor* rd_getprocessor(void);
REDASM_EXPORT bool rd_decode_prev(RDAddress address, RDInstruction* instr);
REDASM_EXPORT bool rd_decode(RDAddress address, RDInstruction* instr);
REDASM_EXPORT RDAddress rd_normalizeaddress(RDAddress address);
REDASM_EXPORT const char* rd_getregistername(int regid);
REDASM_EXPORT const char* rd_getmnemonic(const RDInstruction* instr);
REDASM_EXPORT bool rd_matchmnemonic(const RDInstruction* instr,
                                    const char* mnem);
