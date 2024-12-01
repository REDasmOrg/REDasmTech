#pragma once

#include <redasm/analyzer.h>
#include <redasm/byte.h>
#include <redasm/common.h>
#include <redasm/engine.h>
#include <redasm/function.h>
#include <redasm/graph.h>
#include <redasm/instruction.h>
#include <redasm/listing.h>
#include <redasm/loader.h>
#include <redasm/processor.h>
#include <redasm/rdil.h>
#include <redasm/renderer.h>
#include <redasm/stream.h>
#include <redasm/surface.h>
#include <redasm/theme.h>
#include <redasm/types.h>
#include <redasm/typing.h>

typedef enum RDLogLevel {
    LOGLEVEL_TRACE,
    LOGLEVEL_DEBUG,
    LOGLEVEL_INFO,
    LOGLEVEL_WARNING,
    LOGLEVEL_ERROR,
    LOGLEVEL_CRITICAL,
    LOGLEVEL_OFF,
} RDLogLevel;

typedef enum RDSetName {
    SN_DEFAULT = 0,
    SN_NOWARN = (1 << 0),
    SN_IMPORT = (1 << 1),
    SN_WEAK = (1 << 2),
    SN_FORCE = (1 << 3),
} RDSetName;

RD_HANDLE(RDContext);
RD_HANDLE(RDBuffer);

typedef struct RDTestResult {
    const RDLoader* loader;
    const RDProcessor* processor;
    RDContext* context;
} RDTestResult;

struct RDMemoryInfo {
    union {
        RDAddress start;
        RDAddress baseaddress;
    };

    union {
        RDAddress end;
        RDAddress end_baseaddress;
    };

    usize size;
    int bits;
};

typedef enum RDSegmentType {
    SEG_UNKNOWN = 0,
    SEG_HASDATA = 1 << 0,
    SEG_HASCODE = 1 << 1,
} RDSegmentType;

typedef struct RDSegment {
    const char* name;
    usize type;
    RDAddress address;
    RDAddress endaddress;
    RDOffset offset;
    RDOffset endoffset;
} RDSegment;

typedef struct RDStringResult {
    const char* type;
    const char* value;
    usize totalsize;
    bool terminated;
} RDStringResult;

typedef void (*RDLogCallback)(const char*, void*);
typedef void (*RDStatusCallback)(const char*, void*);
typedef void (*RDErrorCallback)(const char*, void*);

typedef struct RDInitParams {
    RDLogCallback onlog;
    RDStatusCallback onstatus;
    RDErrorCallback onerror;
    void* userdata;
} RDInitParams;

REDASM_EXPORT bool rd_init(const RDInitParams* params);
REDASM_EXPORT void rd_deinit(void);
REDASM_EXPORT void rd_free(void* obj);
REDASM_EXPORT void rd_setloglevel(RDLogLevel l);
REDASM_EXPORT void rd_log(const char* s);
REDASM_EXPORT void rd_status(const char* s);
REDASM_EXPORT void rd_select(RDContext* context);
REDASM_EXPORT void rd_discard(void);
REDASM_EXPORT bool rd_destroy();
REDASM_EXPORT usize rd_getsegments(const RDSegment** segments);
REDASM_EXPORT usize rd_getbytes(const RDByte** bytes);
REDASM_EXPORT usize rd_memoryread(RDAddress address, char* data, usize n);
REDASM_EXPORT bool rd_setflags(RDAddress address, u32 flags);
REDASM_EXPORT int rd_getbits();
REDASM_EXPORT void rd_setbits(int bits);
REDASM_EXPORT bool rd_checkstring(RDAddress address, RDStringResult* r);
REDASM_EXPORT void rd_memoryinfo(RDMemoryInfo* mi);
REDASM_EXPORT const char* rd_rendertext(RDAddress address);

REDASM_EXPORT bool rd_gettype(RDAddress address, const char* tname, RDValue* v);
REDASM_EXPORT usize rd_getentries(RDAddress** entries);
REDASM_EXPORT bool rd_tick(const RDEngineStatus** s);

REDASM_EXPORT void rd_addsearchpath(const char* path);
REDASM_EXPORT RDBuffer* rd_loadfile(const char* filepath);
REDASM_EXPORT usize rd_test(RDBuffer* buffer, const RDTestResult** result);
REDASM_EXPORT void rd_disassemble();

REDASM_EXPORT bool rd_setcomment(RDAddress address, const char* comment);
REDASM_EXPORT bool rd_settype(RDAddress address, const RDType* type);
REDASM_EXPORT bool rd_settypename(RDAddress address, const char* tname);
REDASM_EXPORT bool rd_setfunction(RDAddress address);
REDASM_EXPORT bool rd_setentry(RDAddress address, const char* name);
REDASM_EXPORT void rd_addref(RDAddress fromaddr, RDAddress toaddr, usize type);

REDASM_EXPORT bool rd_setname(RDAddress address, const char* name, usize flags);
REDASM_EXPORT bool rd_getaddress(const char* name, RDAddress* address);
REDASM_EXPORT const char* rd_getname(RDAddress address);
REDASM_EXPORT const char* rd_getcomment(RDAddress address);
REDASM_EXPORT usize rd_getrefsfrom(RDAddress fromaddr, const RDRef** refs);
REDASM_EXPORT usize rd_getrefsfromtype(RDAddress fromaddr, usize type,
                                       const RDRef** refs);
REDASM_EXPORT usize rd_getrefsto(RDAddress toaddr, const RDRef** refs);
REDASM_EXPORT usize rd_getrefstotype(RDAddress toaddr, usize type,
                                     const RDRef** refs);
REDASM_EXPORT bool rd_addresstosegment(RDAddress address, RDSegment* s);
REDASM_EXPORT bool rd_offsettosegment(RDOffset offset, RDSegment* s);
REDASM_EXPORT bool rd_addresstoffset(RDAddress address, RDOffset* offset);
REDASM_EXPORT bool rd_offsettoaddress(RDOffset offset, RDAddress* address);
REDASM_EXPORT bool rd_isaddress(RDAddress address);
REDASM_EXPORT const char* rd_tohex_n(usize val, usize n);
REDASM_EXPORT const char* rd_tohex(usize val);

REDASM_EXPORT void rdplugin_init();
REDASM_EXPORT void rdplugin_free();
