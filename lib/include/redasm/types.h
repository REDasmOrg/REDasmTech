#pragma once

#include <cstdint>
#include <redasm/types/map.h>
#include <redasm/types/str.h>
#include <redasm/types/strv.h>
#include <redasm/types/vect.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uintmax_t usize;
typedef intmax_t isize;

typedef uintptr_t uptr;
typedef intptr_t iptr;

typedef u64 RDAddress;
typedef u64 RDOffset;

typedef usize LIndex;

#define RD_HANDLE(name)                                                        \
    typedef struct name {                                                      \
        void* ptr;                                                             \
    } name
