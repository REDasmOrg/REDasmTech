#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <redasm/common.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define _dict_c_(a, b) a##b
#define _dict_cc(a, b) _dict_c_(a, b)
#define _dict_var(n) _dict_cc(n, __LINE__)

// Public API
// NOLINTBEGIN
typedef struct DictNode {
    struct DictNode* next;
} DictNode;

struct _Dict;

// clang-format off
typedef bool (*_DictEquals)(const void*, const void*, size_t); 
typedef size_t (*_DictHash)(const void*, size_t);
typedef void (*_DictDestroy)(void*);
// clang-format on

typedef struct _Dict {
    size_t capacity;
    size_t length;
    uint32_t nsize;
    uint32_t ksize;
    uint32_t voff;
    DictNode* buckets;
    _DictEquals equals;
    _DictHash hash;
    _DictDestroy destroykey;
    _DictDestroy destroyvalue;
} _Dict;
// NOLINTEND

#define Dict(T) _Dict

// Public Dict API
#define DictNode(name, K, V)                                                   \
    typedef struct name {                                                      \
        struct DictNode* next;                                                 \
        K key;                                                                 \
        V value;                                                               \
    } name

#define dict_create(T)                                                         \
    _dict_create(sizeof(T), sizeof(((T*)0)->key), offsetof(T, value), false)

#define dict_createstr(T)                                                      \
    _dict_create(sizeof(T), sizeof(((T*)0)->key), offsetof(T, value), true)

REDASM_EXPORT void dict_setequals(_Dict* self, _DictEquals cb);
REDASM_EXPORT void dict_sethash(_Dict* self, _DictHash cb);
REDASM_EXPORT void dict_setdestroykey(_Dict* self, _DictDestroy cb);
REDASM_EXPORT void dict_setdestroyvalue(_Dict* self, _DictDestroy cb);
REDASM_EXPORT bool dict_isempty(const _Dict* self);
REDASM_EXPORT size_t dict_getlength(const _Dict* self);
REDASM_EXPORT size_t dict_getcapacity(const _Dict* self);
REDASM_EXPORT float dict_getloadfactor(const _Dict* self);
REDASM_EXPORT void dict_clear(_Dict* self);
REDASM_EXPORT void dict_destroy(_Dict* self);

#define dict_contains(self, k) (!!_dict_get(self, (const void*)&(k)))
#define dict_get(T, self, k) (const T*)_dict_get(self, (const void*)&(k))
#define dict_getstr(T, self, k) (const T*)_dict_get(self, (const void*)(k))

#define dict_set(T, self, k, v)                                                \
    do {                                                                       \
        DictNode* _dict_var(t) = _dict_createnode(self);                       \
        ((T*)_dict_var(t))->key = k;                                           \
        ((T*)_dict_var(t))->value = v;                                         \
        _dict_set(self, _dict_var(t));                                         \
    } while(0)

#define dict_setstr(T, self, k, v) dict_set(T, self, k, v)

#define dict_foreach(T, n, self)                                               \
    if((self)->buckets)                                                        \
        for(size_t _dict_var(i) = 0; _dict_var(i) < (self)->capacity;          \
            _dict_var(i)++)                                                    \
            for(const T* n = (T*)(self)->buckets[_dict_var(i)].next; n;        \
                (n) = (const T*)(n)->next)

// Private Dict API
// NOLINTBEGIN
REDASM_EXPORT _Dict _dict_create(uint32_t nsize, uint32_t ksize, uint32_t voff,
                                 bool keystr);
REDASM_EXPORT DictNode* _dict_createnode(const _Dict* self);
REDASM_EXPORT const DictNode* _dict_get(const _Dict* self, const void* key);
REDASM_EXPORT void _dict_set(_Dict* self, DictNode* n);
// NOLINTEND

#if defined(__cplusplus)
}
#endif
