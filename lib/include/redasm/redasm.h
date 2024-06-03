#pragma once

#include <redasm/analyzer.h>
#include <redasm/byte.h>
#include <redasm/common.h>
#include <redasm/listing.h>
#include <redasm/loader.h>
#include <redasm/processor.h>
#include <redasm/renderer.h>
#include <redasm/stream.h>
#include <redasm/surface.h>
#include <redasm/theme.h>
#include <redasm/types.h>
#include <redasm/typing.h>

enum RDLogLevel {
    LOGLEVEL_TRACE,
    LOGLEVEL_DEBUG,
    LOGLEVEL_INFO,
    LOGLEVEL_WARNING,
    LOGLEVEL_ERROR,
    LOGLEVEL_CRITICAL,
    LOGLEVEL_OFF,
};

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

enum RDSegmentType {
    SEGMENTTYPE_UNKNOWN = 0,
    SEGMENTTYPE_HASDATA = 1 << 0,
    SEGMENTTYPE_HASCODE = 1 << 1,
};

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

REDASM_EXPORT bool rd_init(void);
REDASM_EXPORT void rd_deinit(void);
REDASM_EXPORT void rd_free(void* obj);
REDASM_EXPORT void rd_setloglevel(RDLogLevel l);
REDASM_EXPORT void rd_select(RDContext* context);
REDASM_EXPORT void rd_discard(void);
REDASM_EXPORT bool rd_destroy();
REDASM_EXPORT usize rd_getsegments(const RDSegment** segments);
REDASM_EXPORT usize rd_getbytes(const RDByte** bytes);
REDASM_EXPORT usize rd_memoryread(RDAddress address, char* data, usize n);
REDASM_EXPORT usize rd_getbits();
REDASM_EXPORT bool rd_checkstring(RDAddress address, RDStringResult* r);
REDASM_EXPORT void rd_memoryinfo(RDMemoryInfo* mi);

REDASM_EXPORT void rd_addsearchpath(const char* path);
REDASM_EXPORT RDBuffer* rd_loadfile(const char* filepath);
REDASM_EXPORT usize rd_test(RDBuffer* buffer, const RDTestResult** result);
REDASM_EXPORT void rd_disassemble();
REDASM_EXPORT void rd_enqueue(RDAddress address);
REDASM_EXPORT void rd_schedule(RDAddress address);

REDASM_EXPORT bool rd_settype(RDAddress address, const char* tname);
REDASM_EXPORT bool rd_setfunction(RDAddress address);
REDASM_EXPORT bool rd_setfunctionas(RDAddress address, const char* name);

REDASM_EXPORT const char* rd_getname(RDAddress address);
REDASM_EXPORT bool rd_addresstosegment(RDAddress address, RDSegment* s);
REDASM_EXPORT bool rd_offsettosegment(RDOffset offset, RDSegment* s);
REDASM_EXPORT bool rd_addresstoffset(RDAddress address, RDOffset* offset);
REDASM_EXPORT bool rd_offsettoaddress(RDOffset offset, RDAddress* address);
REDASM_EXPORT bool rd_isaddress(RDAddress address);
REDASM_EXPORT const char* rd_tohex_n(usize val, usize n);
REDASM_EXPORT const char* rd_tohex(usize val);

REDASM_EXPORT void rdplugin_init();
REDASM_EXPORT void rdplugin_free();
