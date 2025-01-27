#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include "strv.h"
#include <redasm/common.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// NOLINTBEGIN
typedef struct _StrLong {
    size_t capacity;
    char* data;
} _StrLong;
// NOLINTEND

typedef struct Str {
    size_t length;

    union {
        _StrLong l;
        char s[sizeof(_StrLong)];
    };
} Str;

REDASM_EXPORT Str str_create(const char* s);
REDASM_EXPORT Str str_create_n(const char* s, size_t start, size_t n);
REDASM_EXPORT char str_at(const Str* self, size_t n);
REDASM_EXPORT char* str_ref(Str* self, size_t n);
REDASM_EXPORT void str_resize(Str* self, size_t n);
REDASM_EXPORT void str_reserve(Str* self, size_t cap);
REDASM_EXPORT void str_appendstr(Str* self, const Str* s);
REDASM_EXPORT void str_append_n(Str* self, const char* s, size_t n);
REDASM_EXPORT void str_append(Str* self, const char* s);
REDASM_EXPORT bool str_startswithstr(Str* self, const Str* s);
REDASM_EXPORT bool str_startswith_n(Str* self, const char* s, size_t n);
REDASM_EXPORT bool str_startswith(Str* self, const char* s);
REDASM_EXPORT bool str_endswithstr(Str* self, const Str* s);
REDASM_EXPORT bool str_endswith_n(Str* self, const char* s, size_t n);
REDASM_EXPORT bool str_endswith(Str* self, const char* s);
REDASM_EXPORT int str_indexofstr(Str* self, const Str* s);
REDASM_EXPORT int str_indexof_n(Str* self, const char* s, size_t n);
REDASM_EXPORT int str_indexof(Str* self, const char* s);
REDASM_EXPORT int str_lastindexofstr(Str* self, const Str* s);
REDASM_EXPORT int str_lastindexof_n(Str* self, const char* s, size_t n);
REDASM_EXPORT int str_lastindexof(Str* self, const char* s);
REDASM_EXPORT bool str_issmall(const Str* self);
REDASM_EXPORT bool str_isempty(const Str* self);
REDASM_EXPORT size_t str_hash(const Str* self);
REDASM_EXPORT size_t str_getlength(const Str* self);
REDASM_EXPORT size_t str_getcapacity(const Str* self);
REDASM_EXPORT const char* str_ptr(const Str* self);
REDASM_EXPORT bool str_equals(const Str* self, const Str* rhs);
REDASM_EXPORT void str_clear(Str* self);
REDASM_EXPORT void str_destroy(Str* self);

// Strv interface
inline Strv str_view(const Str* s) {
    return strv_create_n(str_ptr(s), 0, str_getlength(s));
}

inline Strv str_sub(const Str* s, size_t start, size_t n) {
    return strv_create_n(str_ptr(s), start, n);
}

#if defined(__cplusplus)
}
#endif
