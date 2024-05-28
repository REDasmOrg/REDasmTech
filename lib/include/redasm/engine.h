#pragma once

#include <redasm/types.h>

typedef struct RDEngineStatus {
    // General
    const char* filepath;
    const char* assembler;
    const char* loader;
    usize filesize;
    bool busy;

    // Time
    u64 analysisstart;
    u64 analysisend;

    // Steps
    const char* const* stepslist;
    usize stepscount;
    usize stepscurrent;

    // Analyzers
    const char* const* analyzerslist;
    usize* analyzersdone;
    usize analyzerscount;
    usize analyzerscurrent;

    // Stats
    usize segmentscount;
    usize functionscount;
    usize labelscount;
    int segmentsdiff;
    int functionsdiff;
    int labelsdiff;
} RDEngineStatus;
