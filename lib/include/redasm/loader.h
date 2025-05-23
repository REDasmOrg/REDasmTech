#pragma once

#include <redasm/common.h>
#include <redasm/environment.h>
#include <redasm/plugin.h>
#include <redasm/types.h>

typedef enum RDLoaderFlags {
    LF_NOMERGECODE = (1u << 0),
    LF_NOAUTORENAME = (1u << 1),
} RDLoaderFlags;

struct RDBuffer;
struct RDLoaderPlugin;
RD_HANDLE(RDLoader);

typedef struct RDLoaderRequest {
    struct RDBuffer* file;
    const char* path;
    const char* name;
    const char* ext;
} RDLoaderRequest;

// clang-format off
typedef bool (*RDLoaderPluginParse)(struct RDLoader*, const RDLoaderRequest*);
typedef bool (*RDLoaderPluginLoad)(struct RDLoader*, RDBuffer*);
typedef const char* (*RDLoaderPluginGetProcessor)(struct RDLoader*);
typedef const char* (*RDLoaderPluginGetCallingConvention)(struct RDLoader*);
typedef const RDEnvironment* (*RDLoaderPluginGetEnvironment)(struct RDLoader*);
// clang-format on

typedef struct RDLoaderPlugin {
    RDPLUGIN_HEADER(RDLoader)
    RDLoaderPluginParse parse;
    RDLoaderPluginLoad load;
    RDLoaderPluginGetProcessor get_processor;
    RDLoaderPluginGetCallingConvention get_callingconvention;
    RDLoaderPluginGetEnvironment get_environment;
} RDLoaderPlugin;

define_slice(RDLoaderPluginSlice, const RDLoaderPlugin*);

REDASM_EXPORT bool rd_registerloader(const RDLoaderPlugin* plugin);
REDASM_EXPORT bool rd_registerloader_ex(const RDLoaderPlugin* plugin,
                                        const char* origin);
REDASM_EXPORT const RDLoaderPluginSlice* rd_getloaderplugins(void);
REDASM_EXPORT const RDLoaderPlugin* rd_getloaderplugin(void);
REDASM_EXPORT RDLoader* rd_getloader(void);
