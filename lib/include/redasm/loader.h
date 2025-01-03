#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

struct RDLoader;

enum {
    LF_NOMERGECODE = (1 << 1),
    LF_NOMERGEDATA = (1 << 2),
    LF_NOAUTORENAME = (1 << 3),

    LF_NOMERGE = LF_NOMERGECODE | LF_NOMERGEDATA,
};

typedef bool (*RDLoaderInit)(RDLoader*);
typedef void (*RDLoaderFree)(RDLoader*);

typedef struct RDLoader {
    const char* id;
    const char* name;
    void* userdata;
    usize flags;

    RDLoaderInit init;
    RDLoaderFree free;
} RDLoader;

REDASM_EXPORT const RDLoader* rd_getloader(void);
REDASM_EXPORT usize rd_getloaders(const RDLoader** loaders);
REDASM_EXPORT void rd_registerloader(const RDLoader* ldr);
