#pragma once

#include <redasm/types.h>

struct RDLoader;

typedef bool (*RDLoaderInit)(RDLoader*);
typedef void (*RDLoaderFree)(RDLoader*);

typedef struct RDLoader {
    const char* name;
    void* userdata;

    RDLoaderInit init;
    RDLoaderFree free;
} RDLoader;
