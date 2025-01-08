#pragma once

#include <redasm/common.h>
#include <redasm/plugin.h>
#include <redasm/types.h>

typedef enum RDLoaderFlags {
    LF_NOMERGECODE = (1u << 0),
    LF_NOMERGEDATA = (1u << 1),
    LF_NOAUTORENAME = (1u << 2),

    LF_NOMERGE = LF_NOMERGECODE | LF_NOMERGEDATA,
} RDLoaderFlags;

RD_HANDLE(RDLoader);

typedef bool (*RDLoaderPluginLoad)(RDLoader*);

typedef struct RDLoaderPlugin {
    RDPLUGIN_HEADER(RDLoader)
    RDLoaderPluginLoad load;
} RDLoaderPlugin;

REDASM_EXPORT bool rd_registerloader(const RDLoaderPlugin* plugin);
REDASM_EXPORT const RDLoaderPlugin** rd_getloaderplugins(usize* n);
REDASM_EXPORT const RDLoaderPlugin* rd_getloaderplugin(void);
REDASM_EXPORT RDLoader* rd_getloader(void);
