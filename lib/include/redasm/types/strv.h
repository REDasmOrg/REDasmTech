#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <redasm/common.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct Strv {
    const char* data;
    size_t length;
} Strv;

REDASM_EXPORT Strv strv_create(const char* s);
REDASM_EXPORT Strv strv_create_n(const char* s, size_t start, size_t n);
REDASM_EXPORT bool strv_isempty(const Strv* self);
REDASM_EXPORT size_t strv_getlength(const Strv* self);
REDASM_EXPORT const char* strv_getdata(const Strv* self);
REDASM_EXPORT bool strv_startswith_str(Strv* self, const Strv* s);
REDASM_EXPORT bool strv_startswith_n(Strv* self, const char* s, size_t n);
REDASM_EXPORT bool strv_startswith(Strv* self, const char* s);
REDASM_EXPORT bool strv_endswith_str(Strv* self, const Strv* s);
REDASM_EXPORT bool strv_endswith_n(Strv* self, const char* s, size_t n);
REDASM_EXPORT bool strv_endswith(Strv* self, const char* s);
REDASM_EXPORT int strv_indexof_str(Strv* self, const Strv* s);
REDASM_EXPORT int strv_indexof_n(Strv* self, const char* s, size_t n);
REDASM_EXPORT int strv_indexof(Strv* self, const char* s);
REDASM_EXPORT int strv_lastindexof_str(Strv* self, const Strv* s);
REDASM_EXPORT int strv_lastindexof_n(Strv* self, const char* s, size_t n);
REDASM_EXPORT int strv_lastindexof(Strv* self, const char* s);
REDASM_EXPORT bool strv_equals(const Strv* self, const Strv* rhs);

// GENERIC
// NOLINTBEGIN
int _cstr_indexof(const char* s1, size_t sz1, const char* s2, size_t sz2);
int _cstr_lastindexof(const char* s1, size_t sz1, const char* s2, size_t sz2);

bool _cstr_startswith(const char* s1, size_t sz1, const char* s2, size_t sz2);
bool _cstr_endswith(const char* s1, size_t sz1, const char* s2, size_t sz2);
// NOLINTEND

#if defined(__cplusplus)
}
#endif
