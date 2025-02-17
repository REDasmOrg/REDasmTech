#pragma once

#include <redasm/segment.h>
#include <redasm/types.h>

typedef struct RDWorkerStatus {
    // General
    const char* filepath;
    usize filesize;

    // Context
    const char* loader;
    const char* processor;
    bool listingchanged;
    bool busy;

    RDSegment* segment;

    struct {
        RDAddress value;
        bool valid;
    } address;

    const char* currentstep;
} RDWorkerStatus;
