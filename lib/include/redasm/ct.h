//    ___  _____
//   / __\/__   \    C Toolkit
//  / /     / /\/    v1.0 - github.com/Dax89
// / /___  / /
// \____/  \/
//
// SPDX-FileCopyrightText: 2025 Antonio Davide Trogu <contact@antoniodavide.dev>
// SPDX-License-Identifier: MIT

#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(_MSC_VER)
#if defined(CT_EXPORTS)
#define CT_API __declspec(dllexport)
#else
#define CT_API __declspec(dllimport)
#endif
#else
#if defined(CT_EXPORTS)
#define CT_API __attribute__((visibility("default")))
#else
#define CT_API
#endif
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NOLINTBEGIN
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef ptrdiff_t isize;
typedef size_t usize;

typedef uintptr_t uptr;
typedef intptr_t iptr;
// NOLINTEND

#if defined(__GNUC__) // GCC, Clang, ICC
#define intrinsic_trap() __builtin_trap()
#define intrinsic_unreachable() __builtin_unreachable()
#define intrinsic_unlikely(x) __builtin_expect(!!(x), 0)
#elif defined(_MSC_VER) // MSVC
#define intrinsic_trap() __debugbreak()
#define intrinsic_unreachable() __assume(false)
#define intrinsic_unlikely(x) (!!(x))
#else
#error "ct.h: unsupported compiler"
#endif

#if defined(_MSC_VER)
#define CT_NORET __declspec(noreturn)
#elif defined(__GNUC__) || defined(__clang__)
#define CT_NORET __attribute__((noreturn))
#else
#define CT_NORET _Noreturn
#endif

// *** Error handling *** //
#if defined(NDEBUG) // Release
CT_NORET static inline void _unreachable_impl(void) { intrinsic_unreachable(); }
#else
CT_NORET static inline void _unreachable_impl(void) { intrinsic_trap(); }
#endif

CT_API CT_NORET void _except_impl(const char* file, int line, const char* fmt,
                                  ...);

#define ct_assume(...)                                                         \
    do {                                                                       \
        if(intrinsic_unlikely(!(__VA_ARGS__))) {                               \
            fprintf(stderr, "[%s:%d] Assume condition failed '%s'\n",          \
                    __FILE__, __LINE__, #__VA_ARGS__);                         \
            intrinsic_trap();                                                  \
        }                                                                      \
    } while(0)

#define ct_unreachable                                                         \
    do {                                                                       \
        fprintf(stderr, "[%s:%d] Unreachable code detected\n", __FILE__,       \
                __LINE__);                                                     \
        _unreachable_impl();                                                   \
    } while(0)

#define ct_except(msg) _except_impl(__FILE__, __LINE__, msg)
#define ct_exceptf(fmt, ...) _except_impl(__FILE__, __LINE__, fmt, __VA_ARGS__)

#define ct_except_if(cond, msg)                                                \
    do {                                                                       \
        if(cond) ct_except(msg);                                               \
    } while(0)

#define ct_exceptf_if(cond, msg, ...)                                          \
    do {                                                                       \
        if(cond) ct_exceptf(msg, __VA_ARGS__);                                 \
    } while(0)
// *** Error handling *** //

// *** Misc utilities *** //
#define ct_containerof(ptr, T, m) ((T*)((char*)(ptr) - offsetof(T, m)))
// NOLINTBEGIN
#define ct_ptrtoint(T, x) ((T)(uptr)(x))
#define ct_inttoptr(x) ((void*)(uptr)(x))
// NOLINTEND

#define define_optional(name, T)                                               \
    typedef struct name {                                                      \
        bool ok;                                                               \
        T value;                                                               \
    } name;                                                                    \
    static inline name name##_some(T v) {                                      \
        name opt;                                                              \
        opt.ok = true;                                                         \
        opt.value = v;                                                         \
        return opt;                                                            \
    }                                                                          \
    static inline name name##_none(void) {                                     \
        name opt;                                                              \
        opt.ok = false;                                                        \
        return opt;                                                            \
    }                                                                          \
// *** Misc utilities *** //

// *** Allocators *** //
typedef void* (*Allocator)(void* ctx, void* ptr, isize osize, isize nsize);

typedef struct LinearArena {
    char* begin;
    char* end;
} LinearArena;

CT_API LinearArena lineararena_create(isize n);
CT_API LinearArena lineararena_createfrom_n(char* p, isize n);
CT_API void lineararena_reset(LinearArena* self);
CT_API void lineararena_destroy(LinearArena* self);

#define lineararena_createfrom(p) lineararena_createfrom_n(p, sizeof(p))

CT_API void* std_allocator(void* ctx, void* ptr, isize osize, isize nsize);
CT_API void* linear_allocator(void* ctx, void* ptr, isize osize, isize nsize);

CT_API void* mem_alloc(isize n, Allocator a, void* ctx);
CT_API void* mem_alloc0(isize n, Allocator a, void* ctx);
CT_API void* mem_realloc(void* ptr, isize osize, isize nsize, Allocator a,
                         void* ctx);
CT_API void* mem_realloc0(void* ptr, isize osize, isize nsize, Allocator a,
                          void* ctx);
CT_API void mem_free(void* ptr, isize n, Allocator a, void* ctx);

#define mem_alloctype_n(T, n, a, ctx) (T*)mem_alloc(sizeof(T) * n, a, ctx)
#define mem_alloctype0_n(T, n, a, ctx) (T*)mem_alloc0(sizeof(T) * n, a, ctx)
#define mem_alloctype(T, a, ctx) mem_alloctype_n(T, 1, a, ctx)
#define mem_alloctype0(T, a, ctx) mem_alloctype0_n(T, 1, a, ctx)

#define mem_realloctype(T, ptr, oldn, newn, a, ctx)                            \
    (T*)mem_realloc(ptr, sizeof(T) * (oldn), sizeof(T) * (newn), a, ctx)

#define mem_realloctype0(T, ptr, oldn, newn, a, ctx)                           \
    (T*)mem_realloc0(ptr, sizeof(T) * (oldn), sizeof(T) * (newn), a, ctx)

#define mem_freetype_n(T, ptr, n, a, ctx)                                      \
    mem_free(ptr, sizeof(T) * (n), a, ctx);

#define mem_freetype(T, ptr, a, ctx) mem_freetype_n(T, ptr, 1, a, ctx)
// *** Allocators *** //

// *** Slice *** //
typedef int (*SliceCompare)(const void*, const void*);
typedef bool (*SlicePredicate)(const void*);

typedef struct BSearchResult {
    bool found;
    isize index;
} BSearchResult;

#define define_slice(name, T)                                                  \
    typedef struct name {                                                      \
        T* data;                                                               \
        isize length;                                                          \
        isize capacity;                                                        \
        Allocator alloc;                                                       \
        void* ctx;                                                             \
    } name

#define slice_clear(self) ((self)->length = 0)
#define slice_empty(self) ((self)->length == 0)
#define slice_reserve(self, n) _slice_reserve(self, n, sizeof(*(self)->data))
#define slice_destroy(self) _slice_destroy(self, sizeof(*(self)->data))
#define slice_erase(self) _slice_erase(self, sizeof(*(self)->data))
#define slice_pop(self) ((self)->data[--(self)->length])
#define slice_isnull(self) (!!(self)->data)
#define slice_first(self) ((self)->data[0])
#define slice_last(self) ((self)->data[(self)->length - 1])

#define slice_insert(self, idx, ...)                                           \
    do {                                                                       \
        _slice_insert(self, idx, sizeof(*(self)->data));                       \
        (self)->data[idx == -1 ? (self)->length++ : (++(self)->length, idx)] = \
            __VA_ARGS__;                                                       \
    } while(0)

#define slice_push(self, ...)                                                  \
    do {                                                                       \
        _slice_grow(self, sizeof(*(self)->data));                              \
        (self)->data[(self)->length++] = __VA_ARGS__;                          \
    } while(0)

#define slice_foreach(it, self)                                                \
    for(isize __slice_##it##_i = (it = NULL, 0);                               \
        __slice_##it##_i < (self)->length &&                                   \
        (((it) = &(self)->data[__slice_##it##_i]), true);                      \
        ++__slice_##it##_i, (++it))

#define slice_sort(self, cmp)                                                  \
    qsort((self)->data, (self)->length, sizeof(*(self)->data),                 \
          (SliceCompare)(cmp));

#define slice_bsearch(self, key, cmp)                                          \
    _slice_bsearch(self, sizeof(*(self)->data), key, (SliceCompare)cmp)

#define slice_stablepartition(self, pred)                                      \
    _slice_stablepartition(self, sizeof(*(self)->data), (SlicePredicate)pred)

CT_API void slice_init(void* self, Allocator a, void* ctx);
CT_API void _slice_reserve(void* self, isize n, isize dsize);
CT_API void _slice_insert(void* self, isize idx, isize dsize);
CT_API void _slice_grow(void* self, isize dsize);
CT_API void _slice_erase(void* self, isize idx, usize dsize);
CT_API void _slice_eraserange(void* self, isize idx, isize n, usize dsize);
CT_API void _slice_destroy(void* self, isize dsize);
CT_API BSearchResult _slice_bsearch(const void* self, isize dsize,
                                    const void* key, SliceCompare cmp);
CT_API isize _slice_stablepartition(const void* self, isize dsize,
                                    SlicePredicate pred);
// *** Slice *** //

// *** String *** //
define_slice(Str, char);

typedef struct StrSplit {
    const Str* source;
    isize pos;
    const char* sep;
    isize nsep;
} StrSplit;

#define str_init slice_init
#define str_isview(self) ((self)->alloc == NULL)
#define str_ismutable(self) ((self)->alloc != NULL)
#define str_isnull(self) slice_isnull
#define str_empty slice_empty
#define str_erase slice_erase
#define str_foreach slice_foreach

CT_API Str str_create_n(const char* s, isize n, Allocator a, void* ctx);
CT_API Str str_view_n(const char* s, isize n);
CT_API Str str_sub(const Str* self, isize start, isize end);
CT_API Str str_dup(const Str* self);
CT_API Str* str_dup_to(const Str* self, Str* dest);
CT_API void str_clear(Str* self);
CT_API void str_ncat(Str* self, const char* s, isize n);
CT_API void str_resize(Str* self, isize n);
CT_API void str_reserve(Str* self, isize n);
CT_API void str_destroy(Str* self);
CT_API void str_eraserange(Str* self, isize start, isize end);
CT_API void str_tolower(Str* self);
CT_API void str_toupper(Str* self);
CT_API void str_trim(Str* self);
CT_API int str_compare(const Str* self, const Str* rhs);
CT_API bool str_equals(const Str* self, const Str* rhs);
CT_API usize _str_hash_n(const char* s, isize n);
CT_API bool _str_startswith_n(const Str* self, const char* s, isize n);
CT_API bool _str_endswith_n(const Str* self, const char* s, isize n);
CT_API isize _str_indexof_n(const Str* self, const char* s, isize n);
CT_API isize _str_lastindexof_n(const Str* self, const char* s, isize n);
CT_API void _str_insert_n(Str* self, isize idx, const char* s, isize n);
CT_API void _str_replace_n(Str* self, const char* from, isize nfrom,
                           const char* to, isize nto);

static inline bool str_startswith(const Str* self, const char* s) {
    return _str_startswith_n(self, s, strlen(s));
}

static inline bool str_endswith(const Str* self, const char* s) {
    return _str_endswith_n(self, s, strlen(s));
}

static inline bool str_startswith_str(const Str* self, const Str* s) {
    return _str_startswith_n(self, s->data, s->length);
}

static inline bool str_endswith_str(const Str* self, const Str* s) {
    return _str_endswith_n(self, s->data, s->length);
}

static isize str_indexof(const Str* self, const char* s) {
    return _str_indexof_n(self, s, strlen(s));
}

static isize str_indexof_str(const Str* self, const Str* s) {
    return _str_indexof_n(self, s->data, s->length);
}

static inline usize str_hash(const Str* self) {
    return _str_hash_n(self->data, self->length);
}

static inline usize str_hash_cstr(const char* s) {
    return _str_hash_n(s, strlen(s));
}

static inline isize str_lastindexof(const Str* self, const char* s) {
    return _str_lastindexof_n(self, s, strlen(s));
}

static inline isize str_lastindexof_str(const Str* self, const Str* s) {
    return _str_lastindexof_n(self, s->data, s->length);
}

static inline bool str_contains(const Str* self, const char* s) {
    return str_indexof(self, s) != -1;
}

static inline bool str_contains_str(const Str* self, const Str* s) {
    return str_indexof_str(self, s) != -1;
}

static inline Str str_create(const char* s, Allocator a, void* ctx) {
    return str_create_n(s, strlen(s), a, ctx);
}

static inline Str str_view(const char* s) { return str_view_n(s, strlen(s)); }

static inline void str_cat(Str* self, const char* s) {
    str_ncat(self, s, strlen(s));
}

static inline void str_push(Str* self, const Str* s) {
    str_ncat(self, s->data, s->length);
}

static inline void str_insert(Str* self, isize idx, const char* s) {
    _str_insert_n(self, idx, s, strlen(s));
}

static inline void str_insert_str(Str* self, isize idx, const Str* s) {
    _str_insert_n(self, idx, s->data, s->length);
}

static inline void str_replace(Str* self, const char* from, const char* to) {
    _str_replace_n(self, from, strlen(from), to, strlen(to));
}

static inline void str_replace_str(Str* self, const Str* from, const Str* to) {
    _str_replace_n(self, from->data, from->length, to->data, to->length);
}

CT_API StrSplit _strsplit_create_n(const Str* self, const char* sep, isize n);
CT_API Str str_split_next(StrSplit* it);

static inline StrSplit strsplit_create(Str* self, const char* sep) {
    return _strsplit_create_n(self, sep, strlen(sep));
}

static inline StrSplit strsplit_create_str(Str* self, const Str* sep) {
    return _strsplit_create_n(self, sep->data, sep->length);
}

#define strsplit_foreach(self, it, sep)                                        \
    for(StrSplit _it##it = strsplit_create((self), (sep));                     \
        _it##it.pos <= (self)->length &&                                       \
        ((it) = str_split_next(&_it##it), true);)

#define strsplit_foreach_str(self, it, sep)                                    \
    for(StrSplit _it##var = strsplit_create_str((self), (sep));                \
        _it##it.pos <= (self)->length &&                                       \
        ((it) = str_split_next(&_it##it), true);)

// *** String *** //

// *** List *** //
typedef struct ListNode {
    struct ListNode* prev;
    struct ListNode* next;
} ListNode;

#define list_item(self, T, m) ct_containerof(self, T, m)
#define list_first(self, T, m) list_item((self)->next, T, m);
#define list_last(self, T, m) list_item((self)->prev, T, m);
#define list_next(self, T, m) list_item((self)->m.next, T, m)
#define list_prev(self, T, m) list_item((self)->m.prev, T, m)

#define list_foreach(it, self, T, m)                                           \
    for(it = list_item((self)->next, T, m); &(it)->m != (self);                \
        it = list_next(it, T, m))

#define list_foreach_safe(it, itnext, self, T, m)                              \
    for(it = list_item((self)->next, T, m),                                    \
    itnext = list_item((self)->next->next, T, m);                              \
        &(it)->m != (self);                                                    \
        it = itnext, itnext = list_item(itnext->m.next, T, m))

#define list_foreach_from(it, self, T, m)                                      \
    for(; &(it)->m != (self); it = list_next(it, T, m))

CT_API void list_init(ListNode* self);
CT_API void list_erase(ListNode* self);
CT_API void list_insert(ListNode* prev, ListNode* next, ListNode* n);
CT_API void list_insert_prev(ListNode* self, ListNode* n);
CT_API void list_insert_next(ListNode* self, ListNode* n);
CT_API ListNode* list_pop_first(ListNode* self);
CT_API ListNode* list_pop_last(ListNode* self);

static inline bool list_empty(const ListNode* self) {
    return self->prev == self->next;
}

static inline isize list_length(const ListNode* self) {
    isize c = 0;
    for(ListNode* n = self->next; n != self; n = n->next)
        ++c;
    return c;
}

// *** List *** //

// *** HList *** //
typedef struct HListNode {
    struct HListNode* next;
    struct HListNode** backlink;
} HListNode;

typedef struct HList {
    HListNode* first;
} HList;

#define hlist_item(self, T, m) ct_containerof(self, T, m)
#define hlist_item_safe(self, T, m) ((self) ? hlist_item(self, T, m) : NULL)
#define hlist_next(self, T, m) hlist_item((self)->m.next, T, m)

#define hlist_foreach(it, self, T, m)                                          \
    for(it = hlist_item_safe((self)->first, T, m); it;                         \
        it = hlist_item_safe((it)->m.next, T, m))

#define hlist_foreach_safe(it, itnext, self, T, m)                             \
    for(it = hlist_item_safe((self)->first, T, m),                             \
    itnext = it ? hlist_item_safe((it)->m.next, T, m) : NULL;                  \
        it;                                                                    \
        it = itnext, itnext = it ? hlist_item_safe((it)->m.next, T, m) : NULL)

CT_API void hlist_insert(HList* self, HListNode* n);
CT_API void hlist_erase(HListNode* n);

static inline bool hlist_empty(const HList* self) {
    return self->first == NULL;
}

static inline isize hlist_length(const HList* self) {
    isize c = 0;
    for(HListNode* n = self->first; n; n = n->next)
        ++c;
    return c;
}
// *** HList *** //

// *** Map *** //
typedef uptr (*HMapHash)(const void*);

#define define_hmap(name, b)                                                   \
    typedef struct name {                                                      \
        HList data[1 << (b)];                                                  \
        HMapHash hash;                                                         \
    } name

#define hmap_hash(self, key) ((self)->hash(key) * 11)
#define hmap_capacity(self) (sizeof((self)->data) / sizeof(*(self)->data))
#define hmap_bits(self) _hmap_bits(hmap_capacity(self))
#define hmap_index(self, k) (hmap_hash(self, k) & (hmap_capacity(self) - 1))
#define hmap_empty(self) _hmap_empty((self)->data, hmap_capacity(self))
#define hmap_length(self) _hmap_length((self)->data, hmap_capacity(self))

#define hmap_set(self, n, k)                                                   \
    hlist_insert(&(self)->data[hmap_index(self, k)], n);

#define hmap_init(self, hashfn)                                                \
    do {                                                                       \
        _hmap_init((self)->data, hmap_capacity(self));                         \
        (self)->hash = hashfn ? (HMapHash)hashfn : _hmap_directhash;           \
    } while(0)

#define hmap_foreach_key(it, self, T, m, k)                                    \
    hlist_foreach(it, &(self)->data[hmap_index(self, k)], T, m)

#define hmap_foreach(it, self, T, m)                                           \
    for(usize __hmap_##it##_i = 0; __hmap_##it##_i < hmap_capacity(self);      \
        ++__hmap_##it##_i)                                                     \
    hlist_foreach(it, &(self)->data[__hmap_##it##_i], T, m)

#define hmap_foreach_safe(it, itnext, self, T, m)                              \
    for(usize __hmap_##it##_i = 0; __hmap_##it##_i < hmap_capacity(self);      \
        ++__hmap_##it##_i)                                                     \
    hlist_foreach_safe(it, itnext, &(self)->data[__hmap_##it##_i], T, m)

#define hmap_get(it, self, T, m, k, matchexpr)                                 \
    hmap_foreach_key(it, self, T, m, k) {                                      \
        if(matchexpr) break;                                                   \
    }

#define hmap_del(it, self, T, m, k, matchexpr)                                 \
    hmap_foreach_key(it, self, T, m, k) {                                      \
        if(matchexpr) {                                                        \
            hlist_del(&it->m);                                                 \
            break;                                                             \
        }                                                                      \
    }

CT_API void _hmap_init(HList* self, usize n);
CT_API unsigned long _hmap_bits(unsigned long cap);
uptr _hmap_directhash(const void* p);

static inline bool _hmap_empty(const HList* self, isize n) {
    for(isize i = 0; i < n; ++i)
        if(!hlist_empty(&self[i])) return false;
    return true;
}

static inline isize _hmap_length(const HList* self, usize cap) {
    isize c = 0;
    for(usize i = 0; i < cap; ++i)
        c += hlist_length(&self[i]);
    return c;
}

// *** Map *** //

// *** Red/Black Tree *** //
typedef struct RBTreeNode {
    usize black;
    struct RBTreeNode* parent;
    struct RBTreeNode* left;
    struct RBTreeNode* right;
} RBTreeNode;

typedef int (*RBTreeNodeCompare)(const RBTreeNode*, const RBTreeNode*);
typedef int (*RBTreeKeyCompare)(const void*, const RBTreeNode*);

typedef struct RBTree {
    RBTreeNode* root;
    RBTreeNodeCompare nodecmp;
    RBTreeKeyCompare keycmp;
    isize length;
} RBTree;

CT_API void rbtree_init(RBTree* self, RBTreeNodeCompare nodecmp,
                        RBTreeKeyCompare nodekey);
CT_API bool rbtree_insert(RBTree* self, RBTreeNode* n);
CT_API void rbtree_erase(RBTree* self, RBTreeNode* n);
CT_API RBTreeNode* rbtree_left_deepest(const RBTreeNode* n);
CT_API RBTreeNode* rbtree_right_deepest(const RBTreeNode* n);
CT_API RBTreeNode* rbtree_first(const RBTree* self);
CT_API RBTreeNode* rbtree_last(const RBTree* self);
CT_API RBTreeNode* rbtree_prev(const RBTreeNode* n);
CT_API RBTreeNode* rbtree_next(const RBTreeNode* n);
CT_API RBTreeNode* rbtree_prev_postorder(const RBTreeNode* n);
CT_API RBTreeNode* rbtree_next_postorder(const RBTreeNode* n);
CT_API RBTreeNode* _rbtree_find(const RBTree* self, const void* k);

/*
================================================================================
 rbtree_replace - Replace a node in-place with another node (same sort key)
================================================================================

This replaces the node `old` with `new` without modifying the structure of
the red-black tree. All parent/child relationships are updated accordingly.

IMPORTANT:
- The new node **must represent the same sort key** as the old one.
- Replacing a node with one that violates ordering (e.g. larger or smaller key)
  will corrupt the tree and cause undefined behavior.
- This is useful for memory migration or in-place upgrades where the tree
  structure remains valid.

This operation does NOT rebalance the tree. It assumes the structure and color
invariants are preserved.
 */
CT_API void rbtree_replace(RBTreeNode** self, RBTreeNode* oldn,
                           RBTreeNode* newn);

static inline RBTreeNode* rbtree_first_postorder(const RBTree* self) {
    ct_assume(self);
    return rbtree_left_deepest(self->root);
}

static inline RBTreeNode* rbtree_last_postorder(const RBTree* self) {
    ct_assume(self);
    return rbtree_right_deepest(self->root);
}

#define rbtree_item(self, T, m) ct_containerof(self, T, m)
#define rbtree_item_safe(self, T, m) (self ? rbtree_item(self, T, m) : NULL)
#define rbtree_find(self, k) _rbtree_find(self, k)
#define rbtree_contains(self, k) (rbtree_find(self, k) != NULL)
#define rbtree_first_item(self, T, m) rbtree_item_safe(rbtree_first(self), T, m)
#define rbtree_last_item(self, T, m) rbtree_item_safe(rbtree_last(self), T, m)
#define rbtree_prev_item(self, T, m) rbtree_item_safe(rbtree_prev(self), T, m)
#define rbtree_next_item(self, T, m) rbtree_item_safe(rbtree_next(self), T, m)

#define rbtree_find_item(self, k, T, m)                                        \
    rbtree_item_safe(rbtree_find(self, k), T, m)

#define rbtree_left_deepest_item(self, T, m)                                   \
    rbtree_item_safe(rbtree_left_deepest(self), T, m)

#define rbtree_right_deepest_item(self, T, m)                                  \
    rbtree_item_safe(rbtree_right_deepest(self), T, m)

#define rbtree_first_postorder_item(self, T, m)                                \
    rbtree_item_safe(rbtree_first_postorder(self), T, m)

#define rbtree_last_postorder_item(self, T, m)                                 \
    rbtree_item_safe(rbtree_last_postorder(self), T, m)

#define rbtree_prev_postorder_item(self, T, m)                                 \
    rbtree_item_safe(rbtree_prev_postorder(self), T, m)

#define rbtree_next_postorder_item(self, T, m)                                 \
    rbtree_item_safe(rbtree_next_postorder(self), T, m)

#define rbtree_foreach(it, self, T, m)                                         \
    for(it = rbtree_first_item((self), T, m); it;                              \
        it = rbtree_next_item(&(it)->m, T, m))

#define rbtree_foreach_reverse(it, self, T, m)                                 \
    for(it = rbtree_last_item((self), T, m); it;                               \
        it = rbtree_prev_item(&(it)->m, T, m))

#define rbtree_foreach_safe(it, itnext, self, T, m)                            \
    for(it = rbtree_first_postorder_item((self), T, m), itnext = NULL;         \
        it && (itnext = rbtree_next_postorder_item(&it->m, T, m), 1);          \
        it = itnext)

#define rbtree_foreach_reverse_safe(it, itprev, self, T, m)                    \
    for(it = rbtree_last_postorder_item((self), T, m), itprev = NULL;          \
        it && (itprev = rbtree_prev_postorder_item(&it->m, T, m), 1);          \
        it = itprev)

// *** Red/Black Tree *** //

#if defined(__cplusplus)
}
#endif
