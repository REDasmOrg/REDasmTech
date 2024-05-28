#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

struct RDAnalyzer;

typedef bool (*RDAnalyzerIsEnabled)(const RDAnalyzer*);
typedef void (*RDAnalyzerExecute)(const RDAnalyzer*);
typedef void (*RDAnalyzerFree)(const RDAnalyzer*);

typedef struct RDAnalyzer {
    const char* name;
    usize order;
    void* userdata;

    RDAnalyzerIsEnabled isenabled;
    RDAnalyzerExecute execute;
    RDAnalyzerFree free;
} RDAnalyzer;

REDASM_EXPORT usize rd_getanalyzers(const RDAnalyzer** analyzers);
REDASM_EXPORT bool rdanalyzer_select(const RDAnalyzer* self, bool select);
REDASM_EXPORT bool rdanalyzer_isselected(RDAnalyzer* self);
REDASM_EXPORT void rd_analyze(void);
