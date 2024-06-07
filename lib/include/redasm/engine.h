#pragma once

#include <redasm/types.h>
#include <time.h>

enum RDAnalysisStep {
    STEP_INIT = 0,
    STEP_EMULATE,
    STEP_CFG,
    STEP_ANALYZE,
    STEP_DONE,
    STEP_LAST
};

typedef struct RDAnalysisStatus {
    // General
    const char* filepath;
    usize filesize;

    // Context
    const char* loader;
    const char* processor;
    bool busy;

    struct {
        RDAddress value;
        bool valid;
    } address;

    // Time
    time_t analysisstart;
    time_t analysisend;

    // Steps
    const char* const* stepslist;
    usize stepscount;
    usize stepscurrent;

    // Analyzers
    const char* const* analyzerslist;
    usize* analyzersdone;
    usize analyzerscount;
    usize analyzerscurrent;
} RDEngineStatus;
