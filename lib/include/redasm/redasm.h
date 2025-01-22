#pragma once

#include <redasm/analyzer.h>
#include <redasm/buffer.h>
#include <redasm/byte.h>
#include <redasm/byteorder.h>
#include <redasm/common.h>
#include <redasm/function.h>
#include <redasm/graph.h>
#include <redasm/instruction.h>
#include <redasm/listing.h>
#include <redasm/loader.h>
#include <redasm/memory.h>
#include <redasm/plugin.h>
#include <redasm/processor.h>
#include <redasm/rdil.h>
#include <redasm/renderer.h>
#include <redasm/segment.h>
#include <redasm/stream.h>
#include <redasm/surface.h>
#include <redasm/theme.h>
#include <redasm/types.h>
#include <redasm/typing.h>
#include <redasm/ui.h>
#include <redasm/utils.h>
#include <redasm/version.h>
#include <redasm/worker.h>

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
    SN_NOWARN = (1 << 0),
    SN_IMPORT = (1 << 1),
    SN_FORCE = (1 << 2),
} RDSetName;

typedef enum RDSetType {
    ST_WEAK = (1 << 0),
} RDSetType;

typedef struct RDProblem {
    RDAddress address;
    const char* problem;
} RDProblem;

RD_HANDLE(RDContext);

typedef struct RDTestResult {
    const RDLoaderPlugin* loaderplugin;
    const RDProcessorPlugin* processorplugin;
    RDContext* context;
} RDTestResult;

typedef void (*RDLogCallback)(const char*, void*);
typedef void (*RDStatusCallback)(const char*, void*);
typedef void (*RDErrorCallback)(const char*, void*);

typedef struct RDInitParams {
    RDLogCallback onlog;
    RDStatusCallback onstatus;
    RDErrorCallback onerror;
    RDUI ui;
    void* userdata;
} RDInitParams;

REDASM_EXPORT bool rd_init(const RDInitParams* params);
REDASM_EXPORT void rd_deinit(void);
REDASM_EXPORT void rd_free(void* obj);
REDASM_EXPORT void rd_setloglevel(RDLogLevel l);
REDASM_EXPORT void rd_addsearchpath(const char* sp);
REDASM_EXPORT usize rd_getsearchpaths(const char*** spaths);
REDASM_EXPORT void rd_setuserdata(const char* k, uptr v);
REDASM_EXPORT bool rd_getuserdata(const char* k, uptr* v);
REDASM_EXPORT void rd_log(const char* s);
REDASM_EXPORT void rd_status(const char* s);
REDASM_EXPORT void rd_select(const RDTestResult* tr);
REDASM_EXPORT void rd_discard(void);
REDASM_EXPORT bool rd_destroy(void);
REDASM_EXPORT usize rd_getsegments(const RDSegment** segments);
REDASM_EXPORT usize rd_getmemory(const RDByte** data);
REDASM_EXPORT usize rd_getfile(const u8** data);
REDASM_EXPORT const char* rd_rendertext(RDAddress address);

REDASM_EXPORT usize rd_getentries(RDAddress** entries);
REDASM_EXPORT bool rd_tick(const RDWorkerStatus** s);

REDASM_EXPORT void rd_addsearchpath(const char* path);
REDASM_EXPORT RDBuffer* rd_loadfile(const char* filepath);
REDASM_EXPORT usize rd_test(RDBuffer* buffer, RDTestResult** result);
REDASM_EXPORT void rd_disassemble(void);
REDASM_EXPORT usize rd_getproblems(const RDProblem** problems);

REDASM_EXPORT bool rd_setcomment(RDAddress address, const char* comment);
REDASM_EXPORT bool rd_settype(RDAddress address, const RDType* type,
                              RDValue* v);
REDASM_EXPORT bool rd_settype_ex(RDAddress address, const RDType* type,
                                 usize flags, RDValue* v);
REDASM_EXPORT bool rd_settypename(RDAddress address, const char* tname,
                                  RDValue* v);
REDASM_EXPORT bool rd_settypename_ex(RDAddress address, const char* tname,
                                     usize flags, RDValue* v);

REDASM_EXPORT bool rd_maptype(RDOffset offset, RDAddress address,
                              const RDType* t);
REDASM_EXPORT bool rd_maptypename(RDOffset offset, RDAddress address,
                                  const char* tname);
REDASM_EXPORT bool rd_maptype_ex(RDOffset offset, RDAddress address,
                                 const RDType* t, usize flags);
REDASM_EXPORT bool rd_maptypename_ex(RDOffset offset, RDAddress address,
                                     const char* tname, usize flags);
REDASM_EXPORT bool rd_setfunction(RDAddress address);
REDASM_EXPORT bool rd_setfunction_ex(RDAddress address, usize flags);
REDASM_EXPORT bool rd_setentry(RDAddress address, const char* name);
REDASM_EXPORT void rd_addref(RDAddress fromaddr, RDAddress toaddr, usize type);
REDASM_EXPORT void rd_addproblem(RDAddress address, const char* problem);

REDASM_EXPORT bool rd_setname(RDAddress address, const char* name);
REDASM_EXPORT bool rd_setname_ex(RDAddress address, const char* name,
                                 usize flags);

REDASM_EXPORT bool rd_gettypename(RDAddress address, const char* tname,
                                  RDValue* v);
REDASM_EXPORT bool rd_gettype(RDAddress address, const RDType* t, RDValue* v);

REDASM_EXPORT bool rd_mapsegment(const char* name, RDAddress address,
                                 RDAddress endaddress, RDOffset offset,
                                 RDOffset endoffset, usize type);

REDASM_EXPORT bool rd_mapsegment_n(const char* name, RDAddress address,
                                   usize asize, RDOffset offset, usize osize,
                                   usize type);

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
REDASM_EXPORT bool rd_addresstooffset(RDAddress address, RDOffset* offset);
REDASM_EXPORT bool rd_addresstoindex(RDAddress address, MIndex* index);
REDASM_EXPORT bool rd_offsettoaddress(RDOffset offset, RDAddress* address);

REDASM_EXPORT void rdplugin_create(void);
REDASM_EXPORT void rdplugin_destroy(void);
