// __   _____ ___ _____
// \ \ / / __/ __|_   _|  Vector implementation for C
//  \ V /| _| (__  | |    Version: 1.0
//   \_/ |___\___| |_|    https://github.com/Dax89
//
// SPDX-FileCopyrightText: 2025 Antonio Davide Trogu <contact@antoniodavide.dev>
// SPDX-License-Identifier: MIT

#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NOLINTBEGIN
typedef void* (*VectAlloc)(void* ctx, void* ptr, uintptr_t osize,
                           uintptr_t nsize);

typedef void (*VectItemDel)(void*);

typedef struct VectHeader {
    void* ctx;
    VectAlloc alloc;
    uintptr_t length;
    uintptr_t capacity;
    uintptr_t itemsize;
    VectItemDel itemdel;
    char items[1];
} VectHeader;

#define Vect(T) T*

// clang-format off
#define vect_header(self) ((VectHeader*)(((char*)(self)) - offsetof(VectHeader, items)))
#define vect_alloc_n(T, ctx, allocfn, n) (Vect(T))_vect_alloc_n(ctx, allocfn, sizeof(T), n)
#define vect_alloc(T, ctx, allocfn) vect_alloc_n(T, ctx, allocfn, 0)
#define vect_create_n(T, n) vect_alloc_n(T, NULL, _vect_defaultalloc, n)
#define vect_create(T) vect_alloc(T, NULL, _vect_defaultalloc)
#define vect_begin(self) (self)
#define vect_end(self) ((self) + vect_header(self)->length)
#define vect_first(self) ((self)[0])
#define vect_last(self) ((self)[vect_header(self)->length - 1])
#define vect_resize(T, self, n) self = (Vect(T))_vect_resize(self, n))
#define vect_reserve(T, self, cap) self = (Vect(T))_vect_reserve(self, cap)
#define vect_length(self) ((self) ? vect_header(self)->length : 0)
#define vect_capacity(self) ((self) ? vect_header(self)->capacity : 0)
#define vect_empty(self) (!(self) || !vect_header(self)->length)
#define vect_setitemdel(self, itemdelfn) vect_header(self)->itemdel = itemdelfn
#define vect_clear(self) _vect_clear((void*)(self))
#define vect_destroy(self) _vect_destroy((void*)(self))

#define vect_ins(T, self, idx, ...) do {            \
    self = (Vect(T))_vect_ins(self, idx);           \
    self[idx] = __VA_ARGS__;                        \
} while(0)

#define vect_add(T, self, ...) do {                 \
    self = (Vect(T))_vect_ins(self, (uintptr_t)-1); \
    self[vect_length(self) - 1] = __VA_ARGS__;      \
} while(0)

#define vect_pop_n(self, n) do {                    \
    VectHeader* hdr = vect_header(self);            \
    if(hdr->length > (n)) hdr->length -= (n);       \
    else hdr->length = 0; \
} while(0)

#define vect_pop(self) vect_pop_n(self, 1)

#define vect_foreach(T, item, self)                 \
    if(self)                                        \
        for(T* item = vect_begin(self); item != vect_end(self); item++) // NOLINT

// clang-format on

inline VectAlloc vect_getallocator(Vect(void) self, void** ctx) {
    VectHeader* hdr = vect_header(self);
    if(ctx) *ctx = hdr->ctx;
    return hdr->alloc;
}

inline void* _vect_defaultalloc(void* ctx, void* ptr, uintptr_t osize,
                                uintptr_t nsize) {
    (void)ctx;

    if(nsize == 0) {
        free(ptr);
        return NULL;
    }

    void* p = calloc(1, nsize);

    if(!p) {
        fprintf(stderr, "Vect allocation failed\n");
        abort();
        return NULL;
    }

    if(ptr) {
        memcpy(p, ptr, osize);
        free(ptr);
    }

    return p;
}

inline Vect(void)
    _vect_alloc_n(void* ctx, VectAlloc alloc, uintptr_t itemsize, uintptr_t n) {
    uintptr_t cap = (n ? n : sizeof(uintptr_t)) << 1;
    VectHeader* hdr =
        (VectHeader*)alloc(ctx, NULL, 0, sizeof(VectHeader) + (itemsize * cap));
    hdr->ctx = ctx;
    hdr->alloc = alloc;
    hdr->length = n;
    hdr->capacity = cap;
    hdr->itemsize = itemsize;
    return hdr->items;
}

inline void _vect_clear(Vect(void) self) {
    VectHeader* hdr = vect_header(self);

    if(hdr->itemdel) {
        char* begin = (char*)self;
        char* end = (char*)self + (hdr->length * hdr->itemsize);

        for(char* it = begin; it != end; it += hdr->itemsize)
            hdr->itemdel(it);
    }

    vect_header(self)->length = 0;
}

inline void _vect_destroy(Vect(void) self) {
    if(!self) return;
    VectHeader* hdr = vect_header(self);
    _vect_clear(self);
    hdr->alloc(hdr->ctx, hdr,
               sizeof(VectHeader) + (hdr->itemsize * hdr->capacity), 0);
}

inline Vect(void) _vect_reserve(Vect(void) self, uintptr_t newcap) {
    VectHeader* hdr = vect_header(self);
    if(hdr->capacity >= newcap) return self;

    VectHeader* newhdr = (VectHeader*)hdr->alloc(
        hdr->ctx, hdr, sizeof(VectHeader) + (hdr->itemsize * hdr->capacity),
        sizeof(VectHeader) + (hdr->itemsize * newcap));
    newhdr->capacity = newcap;
    return newhdr->items;
}

inline Vect(void) _vect_resize(Vect(void) self, uintptr_t newn) {
    self = _vect_reserve(self, newn);
    vect_header(self)->length = newn;
    return self;
}

inline Vect(void) _vect_ins(Vect(void) self, uintptr_t idx) {
    uintptr_t len = vect_header(self)->length;
    if(idx == (uintptr_t)-1) idx = len; // Append mode
    assert(idx <= len && "Index out of range in _vect_ins()");

    uintptr_t newlen = len + 1;
    self = _vect_reserve(self, newlen);
    VectHeader* hdr = vect_header(self);

    // If we are appending (pos == len), skip memmove
    if(idx < len) {
        // Shift existing content after the position
        // (+1 to include null terminator)
        memmove((char*)self + ((idx + 1) * hdr->itemsize),
                (char*)self + (idx * hdr->itemsize),
                (len - idx) * hdr->itemsize);
    }

    hdr->length = newlen; // Update the length
    return self;
}

inline void vect_del(Vect(void) self, uintptr_t idx) {
    VectHeader* hdr = vect_header(self);
    uintptr_t len = hdr->length;
    assert(idx < len); // Index is out of range

    if(hdr->itemdel) hdr->itemdel((char*)self + (idx * hdr->itemsize));

    // Shift elements left if not removing the last element
    if(idx < len - 1) {
        memmove((char*)self + (idx * hdr->itemsize),
                (char*)self + ((idx + 1) * hdr->itemsize),
                (len - idx - 1) * hdr->itemsize);
    }

    hdr->length--; // Update length
}

inline void vect_del_n(Vect(void) self, uintptr_t start, uintptr_t n) {
    VectHeader* hdr = vect_header(self);
    if(start >= hdr->length || n <= 0) return;

    uintptr_t end = start + n;
    if(end > hdr->length) end = hdr->length;

    // Shift remaining elements left
    memmove((char*)self + (start * hdr->itemsize),
            (char*)self + (end * hdr->itemsize),
            (hdr->length - end) * hdr->itemsize);

    hdr->length -= (end - start);
}
// NOLINTEND

#if defined(__cplusplus)
}
#endif
