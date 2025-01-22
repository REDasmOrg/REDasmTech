#pragma once

#include <redasm/common.h>
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
    const char* path;
    const char* name;
    const char* ext;
    struct RDBuffer* file;
} RDLoaderRequest;

// clang-format off
typedef bool (*RDLoaderPluginAccept)(const struct RDLoaderPlugin*, const RDLoaderRequest*);
typedef bool (*RDLoaderPluginLoad)(struct RDLoader*, RDBuffer*);
typedef const char* (*RDLoaderPluginGetProcessor)(struct RDLoader*);
// clang-format on

typedef struct RDLoaderPlugin {
    RDPLUGIN_HEADER(RDLoader)
    RDLoaderPluginAccept accept;
    RDLoaderPluginLoad load;
    RDLoaderPluginGetProcessor get_processor;
} RDLoaderPlugin;

REDASM_EXPORT bool rd_registerloader(const RDLoaderPlugin* plugin);
REDASM_EXPORT const RDLoaderPlugin** rd_getloaderplugins(usize* n);
REDASM_EXPORT const RDLoaderPlugin* rd_getloaderplugin(void);
REDASM_EXPORT RDLoader* rd_getloader(void);
