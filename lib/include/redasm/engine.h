#pragma once

#include <redasm/types.h>
#include <time.h>

typedef struct RDAnalysisStatus {
    // General
    const char* filepath;
    usize filesize;

    // Context
    const char* loader;
    const char* processor;
    bool listingchanged;
    bool busy;

    struct {
        RDAddress value;
        bool valid;
    } address;

    // Time
    time_t analysisstart;
    time_t analysisend;

    // Steps
    const char* currentstep;

    // Analyzers
    const char* analyzerslist;
    usize* analyzersdone;
    usize analyzerscount;
    usize analyzerscurrent;
} RDEngineStatus;
