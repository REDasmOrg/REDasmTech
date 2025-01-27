#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <redasm/common.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Public API
// NOLINTBEGIN
typedef void (*VectDestroyItem)(void*);

typedef struct _Vect {
    size_t capacity;
    size_t length;
    size_t esize;
    void* data;
    VectDestroyItem destroyitem;
} _Vect;
// NOLINTEND

#define Vect(T) _Vect

#define vect_create(T) _vect_create(sizeof(T), 0)
#define vect_create_n(T, n) _vect_create(sizeof(T), n)
#define vect_at(T, self, idx) ((const T*)_vect_at((self), idx))
#define vect_ref(T, self, idx) ((T*)_vect_ref((self), idx))

#define vect_append(T, self, ...)                                              \
    ((T*)_vect_checkcapacity(self))[(self)->length++] = __VA_ARGS__

#define vect_foreach(T, n, self)                                               \
    if((self)->data)                                                           \
        for(T* n = (T*)((self)->data);                                         \
            (n) < ((T*)((self)->data) + (self)->length); (n)++)

REDASM_EXPORT void vect_setdestroyitem(_Vect* self, VectDestroyItem cb);
REDASM_EXPORT bool vect_isempty(const _Vect* self);
REDASM_EXPORT size_t vect_getlength(const _Vect* self);
REDASM_EXPORT size_t vect_getcapacity(const _Vect* self);
REDASM_EXPORT void vect_reserve(_Vect* self, size_t n);
REDASM_EXPORT void vect_clear(_Vect* self);
REDASM_EXPORT void vect_destroy(_Vect* self);

// Private API
// NOLINTBEGIN
REDASM_EXPORT _Vect _vect_create(size_t esize, size_t cap);
REDASM_EXPORT void* _vect_checkcapacity(_Vect* self);
REDASM_EXPORT const void* _vect_at(const _Vect* self, size_t idx);
REDASM_EXPORT void* _vect_ref(_Vect* self, size_t idx);
// NOLINTEND

#if defined(__cplusplus)
}
#endif
