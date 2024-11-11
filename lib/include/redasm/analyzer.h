#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

struct RDAnalyzer;

typedef enum RDAnalyzerFlags {
    ANALYZER_NONE = 0,
    ANALYZER_SELECTED = (1 << 0),
    ANALYZER_RUNONCE = (1 << 1),
    ANALYZER_EXPERIMENTAL = (1 << 2),
} RDAnalyzerFlags;

typedef bool (*RDAnalyzerIsEnabled)(const RDAnalyzer*);
typedef void (*RDAnalyzerExecute)(const RDAnalyzer*);
typedef void (*RDAnalyzerFree)(const RDAnalyzer*);

typedef struct RDAnalyzer {
    const char* name;
    usize flags;
    usize order;
    void* userdata;

    RDAnalyzerIsEnabled isenabled;
    RDAnalyzerExecute execute;
    RDAnalyzerFree free;
} RDAnalyzer;

REDASM_EXPORT usize rd_getanalyzers(const RDAnalyzer** analyzers);
REDASM_EXPORT bool rdanalyzer_select(const RDAnalyzer* self, bool select);
REDASM_EXPORT bool rdanalyzer_isselected(const RDAnalyzer* self);
