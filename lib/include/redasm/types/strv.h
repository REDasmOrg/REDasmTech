//  ___ _____ _____   __
// / __|_   _| _ \ \ / /  StringView implementation for C
// \__ \ | | |   /\ V /   Version: 1.0
// |___/ |_| |_|_\ \_/    https://github.com/Dax89
//
// SPDX-FileCopyrightText: 2025 Antonio Davide Trogu <contact@antoniodavide.dev>
// SPDX-License-Identifier: MIT

#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// NOLINTBEGIN
typedef struct StrV {
    const char* ptr;
    uintptr_t length;
} StrV;

// clang-format off
#define strv_npos (uintptr_t)-1
#define strv_lit(s) strv_create_n(s, sizeof(s) - 1)
#define strv_create(s) strv_create_n(s, strlen(s))
#define strv_begin(self) ((self)->ptr)
#define strv_end(self) ((self)->ptr + (self)->length - 1)
#define strv_first(self) ((self)->ptr[0])
#define strv_last(self) ((self)->ptr[(self)->length - 1])
#define strv_valid(self) (!!(self).ptr)
#define strv_length(self) ((self).length)
#define strv_ptr(self) ((self).ptr)
#define strv_empty(self) (!(self).ptr || !(self).length)
#define strv_equals(self, rhs) strv_equals_n(self, rhs, strlen(rhs))
#define strv_equals_lit(self, rhs) strv_equals_n(self, rhs, sizeof(rhs) - 1)
#define strv_startswith(self, s) strv_startswith_n(self, s, strlen(s))
#define strv_startswith_lit(self, s) strv_startswith_n(self, s, sizeof(s) - 1)
#define strv_endswith(self, s) strv_endswith_n(self, s, strlen(s))
#define strv_endswith_lit(self, s) strv_endswith_n(self, s, sizeof(s) - 1)
#define strv_index(self, s) _cstr_index_n((self).ptr, (self).length, s, strlen(s))
#define strv_index_lit(self, s) _cstr_index((self).ptr, (self).length, s, sizeof(s) - 1)
#define strv_lastindex(self, s) _cstr_lastindex_n((self).ptr, (self).length, s, strlen(s))
#define strv_lastindex_lit(self, s) _cstr_lastindex((self).ptr, (self).length, s, sizeof(s) - 1)
#define strv_contains(self, s) (strv_index(self, s) != strv_npos)
#define strv_split(self, sep) _strv_split_n(self, sep, strlen(sep))
#define strv_split_lit(self, sep) _strv_split_n(self, sep, sizeof(sep) - 1)

#define strv_rpop_n(self, n) do { \
    if ((n) <= 0 || ((self)->length) <= 0) break; \
    if ((n) >= (self)->length) hdr->length = 0; \
    else { \
        (self) = (self)->ptr * (n)); \
        (self)->length -= (n); \
    } \
} while(0)

#define strv_rpop(self) strv_rpop_n(self, 1)

#define strv_pop_n(self, n) do { \
    if((self)->length > (n)) (self)->length -= (n); \
    else (self)->length = 0; \
} while(0)

#define strv_pop(self) strv_pop_n(self, 1)

#define strv_foreach_split(it, self, sep) \
    for(StrV it##_ = (self), it = strv_split(&it##_, sep); strv_valid(it); it = strv_split(&it##_, sep)) // NOLINT
                                                                                                         //
#define strv_foreach_split_lit(it, self, sep) \
    for(StrV it##_ = (self), it = strv_split_lit(&it##_, sep); strv_valid(it); it = strv_split_lit(&it##_, sep)) // NOLINT

#define strv_foreach(it, self) \
    for(const char* it = strv_begin(self); it != strv_end(self); it++) // NOLINT

// clang-format on

inline uintptr_t _cstr_index_n(const char* s1, uintptr_t n1, const char* s2,
                               uintptr_t n2) {
    if(!s1 || !s2 || n2 > n1) return strv_npos;

    for(uintptr_t i = 0; i <= n1 - n2; ++i) {
        bool found = true;
        for(uintptr_t j = 0; j < n2; ++j) {
            if(s1[i + j] == s2[j]) continue;
            found = false;
            break;
        }
        if(found) return i;
    }

    return strv_npos;
}

inline uintptr_t _cstr_lastindex_n(const char* s1, uintptr_t n1, const char* s2,
                                   uintptr_t n2) {
    if(!s1 || !s2 || n2 > n1) return strv_npos;

    for(uintptr_t i = n1 - n2; i-- > 0;) {
        bool found = true;
        for(uintptr_t j = 0; j < n2; ++j) {
            if(s1[i + j] == s2[j]) continue;
            found = false;
            break;
        }
        if(found) return i;
    }

    return strv_npos;
}

inline StrV strv_create_n(const char* s, uintptr_t n) {
    StrV self;
    self.ptr = s;
    self.length = n;
    return self;
}

inline bool strv_equals_n(StrV self, const char* rhs, uintptr_t n) {
    if(self.length != n) return false;
    return !memcmp(self.ptr, rhs, self.length);
}

inline StrV _strv_split_n(StrV* self, const char* sep, uintptr_t n) {
    if(self->ptr) {
        uintptr_t idx = _cstr_index_n(self->ptr, self->length, sep, n);

        if(idx != strv_npos) {
            StrV sp = strv_create_n(self->ptr, idx);
            self->ptr += idx + n;
            self->length -= idx + n;
            return sp;
        }
    }

    return {NULL, 0};
}

inline bool strv_startswith_n(StrV self, const char* prefix, uintptr_t n) {
    if(self.length < n) return false;
    return !memcmp(self.ptr, prefix, n);
}

inline bool strv_endswith_n(StrV self, const char* suffix, uintptr_t n) {
    if(self.length < n) return false;
    uintptr_t off = self.length - n;
    return !memcmp(self.ptr + off, suffix, n);
}

inline StrV strv_sub(StrV self, uintptr_t start, uintptr_t end) {
    StrV res = {NULL, 0};

    if(strv_valid(self) && start < end && end < self.length) {
        res.ptr = self.ptr + start;
        res.length = end - start;
    }

    return res;
}

// NOLINTEND

#if defined(__cplusplus)
}
#endif
