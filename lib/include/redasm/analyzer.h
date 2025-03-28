#pragma once

#include <redasm/common.h>
#include <redasm/plugin.h>
#include <redasm/types.h>

typedef enum RDAnalyzerFlags {
    AF_SELECTED = (1 << 0),
    AF_RUNONCE = (1 << 1),
    AF_EXPERIMENTAL = (1 << 2),
} RDAnalyzerFlags;

RD_HANDLE(RDAnalyzer);
struct RDAnalyzerPlugin;

typedef bool (*RDAnalyzerPluginIsEnabled)(const struct RDAnalyzerPlugin*);
typedef void (*RDAnalyzerPluginExecute)(RDAnalyzer*);

typedef struct RDAnalyzerPlugin {
    RDPLUGIN_HEADER(RDAnalyzer)
    u32 order;
    RDAnalyzerPluginIsEnabled is_enabled;
    RDAnalyzerPluginExecute execute;
} RDAnalyzerPlugin;

define_slice(RDAnalyzerPluginSlice, const RDAnalyzerPlugin*);

REDASM_EXPORT bool rd_registeranalyzer(const RDAnalyzerPlugin* plugin);
REDASM_EXPORT bool rd_registeranalyzer_ex(const RDAnalyzerPlugin* plugin,
                                          const char* origin);
REDASM_EXPORT const RDAnalyzerPluginSlice* rd_getanalyzerplugins(void);
REDASM_EXPORT const RDAnalyzerPlugin** rd_getanalyzers(usize* n);
REDASM_EXPORT bool rdanalyzerplugin_select(const RDAnalyzerPlugin* self,
                                           bool select);
REDASM_EXPORT bool rdanalyzerplugin_isselected(const RDAnalyzerPlugin* self);
