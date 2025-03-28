#pragma once

#include <redasm/ct.h>

typedef u64 RDAddress;
typedef u64 RDOffset;

typedef usize LIndex;

#define RD_HANDLE(name)                                                        \
    typedef struct name {                                                      \
        void* ptr;                                                             \
    } name
