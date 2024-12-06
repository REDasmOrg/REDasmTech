#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

struct RDLoader;

typedef bool (*RDLoaderInit)(RDLoader*);
typedef void (*RDLoaderFree)(RDLoader*);

typedef struct RDLoader {
    const char* id;
    const char* name;
    void* userdata;

    RDLoaderInit init;
    RDLoaderFree free;
} RDLoader;

REDASM_EXPORT usize rd_getloaders(const RDLoader** loaders);
