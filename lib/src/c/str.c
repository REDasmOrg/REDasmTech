#include <redasm/types/str.h>
#include <stdlib.h>
#include <string.h>

#define STR_LONGBIT ((size_t)1 << (sizeof(size_t) * 8 - 1))
#define STR_LENMASK ~((size_t)1 << (sizeof(size_t) * 8 - 1))
#define STR_CHECKSMALL(self, len) (len + 1 < sizeof((self)->s))

Str str_create(const char* s) { return str_create_n(s, 0, s ? strlen(s) : 0); }

Str str_create_n(const char* s, size_t start, size_t n) {
    Str self = {0};
    str_reserve(&self, n);

    if(s) {
        if(!str_issmall(&self)) {
            memcpy(self.l.data, s + start, n);
            self.l.data[n] = 0;
            self.length = STR_LONGBIT | n;
        }
        else {
            memcpy(self.s, s + start, n);
            self.length = n;
        }
    }

    return self;
}

char str_at(const Str* self, size_t n) {
    if(!self || self->length >= n) return 0;
    return str_ptr(self)[n];
}

char* str_ref(Str* self, size_t n) {
    if(!self || self->length >= n) return NULL;
    char* p = str_issmall(self) ? self->s : self->l.data;
    return p + n;
}

void str_resize(Str* self, size_t n) {
    if(!self) return;
    str_reserve(self, n);
    self->length = str_issmall(self) ? n : (STR_LONGBIT | n);
}

void str_reserve(Str* self, size_t cap) {
    if(!self) return;

    size_t currcap = str_getcapacity(self);
    if(currcap >= cap) return;

    while(currcap < cap)
        currcap <<= 1;

    if(str_issmall(self) && !STR_CHECKSMALL(self, currcap)) {
        // Small to Long String
        char* newdata = (char*)malloc(currcap + 1);
        if(self->length) memcpy(newdata, &self->s, self->length + 1);
        self->l.data = newdata;
        self->l.capacity = currcap;
        self->length |= STR_LONGBIT;
    }
    else if(!str_issmall(self)) {
        // Expand long string
        char* olddata = self->l.data;
        self->l.data = (char*)malloc(currcap + 1);
        if(self->length) memcpy(self->l.data, olddata, self->length + 1);
        self->l.capacity = currcap;
        free(olddata);
    }
}

void str_append_n(Str* self, const char* s, size_t n) {
    size_t len = str_getlength(self);
    size_t newlen = len + n;
    str_reserve(self, newlen);

    if(str_issmall(self)) {
        memcpy(self->s + len, s, n);
        self->s[newlen] = 0;
        self->length = newlen;
    }
    else {
        memcpy(self->l.data + len, s, n);
        self->l.data[newlen] = 0;
        self->length = STR_LONGBIT | newlen;
    }
}

void str_append(Str* self, const char* s) {
    if(s) str_append_n(self, s, strlen(s));
}

void str_appendstr(Str* self, const Str* s) {
    if(s) str_append_n(self, str_ptr(s), str_getlength(s));
}

bool str_startswithstr(Str* self, const Str* s) {
    return self && s && str_startswith_n(self, str_ptr(s), str_getlength(s));
}

bool str_startswith_n(Str* self, const char* s, size_t n) {
    return self && s &&
           _cstr_startswith(str_ptr(self), str_getlength(self), s, n);
}

bool str_startswith(Str* self, const char* s) {
    return self && s && str_startswith_n(self, s, strlen(s));
}

bool str_endswithstr(Str* self, const Str* s) {
    return self && s && str_endswith_n(self, str_ptr(s), str_getlength(s));
}

bool str_endswith_n(Str* self, const char* s, size_t n) {
    return self && s &&
           _cstr_endswith(str_ptr(self), str_getlength(self), s, n);
}

bool str_endswith(Str* self, const char* s) {
    return self && s && str_endswith_n(self, s, strlen(s));
}

int str_indexofstr(Str* self, const Str* s) {
    if(!self || !s) return -1;
    return str_indexof_n(self, str_ptr(s), str_getlength(s));
}

int str_indexof_n(Str* self, const char* s, size_t n) {
    if(!self) return -1;
    return _cstr_indexof(str_ptr(self), str_getlength(self), s, n);
}

int str_indexof(Str* self, const char* s) {
    if(!self) return -1;
    return s && str_indexof_n(self, s, strlen(s));
}

int str_lastindexofstr(Str* self, const Str* s) {
    if(!self || !s) return -1;
    return str_lastindexof_n(self, str_ptr(s), str_getlength(s));
}

int str_lastindexof_n(Str* self, const char* s, size_t n) {
    if(!self || !s) return -1;
    return _cstr_lastindexof(str_ptr(self), str_getlength(self), s, n);
}

int str_lastindexof(Str* self, const char* s) {
    if(!self) return -1;
    return self && s && str_lastindexof_n(self, s, strlen(s));
}

bool str_issmall(const Str* self) {
    return !self || !(self->length & STR_LONGBIT);
}

bool str_isempty(const Str* self) { return !self || !str_getlength(self); }

// https://stackoverflow.com/questions/7666509/hash-function-for-string
size_t str_hash(const Str* self) {
    if(!self) return 0;

    size_t hash = 5381;
    const char* ptr = str_ptr(self);

    while(*ptr)
        hash = ((hash << 5) + hash) + *ptr++; /* hash * 33 + c */

    return hash;
}

size_t str_getlength(const Str* self) {
    if(!self) return 0;
    return self->length & STR_LENMASK;
}

size_t str_getcapacity(const Str* self) {
    if(!self) return 0;
    return str_issmall(self) ? sizeof(self->s) : self->l.capacity;
}

const char* str_ptr(const Str* self) {
    if(!self) return NULL;
    return str_issmall(self) ? self->s : self->l.data;
}

bool str_equals(const Str* self, const Str* rhs) {
    if(!self || !rhs) return false;
    const char* pl = str_issmall(self) ? self->s : self->l.data;
    const char* pr = str_issmall(rhs) ? rhs->s : rhs->l.data;
    return !strcmp(pl, pr);
}

void str_clear(Str* self) {
    if(!self || !str_getlength(self)) return;

    if(str_issmall(self)) {
        self->length = 0;
        self->s[0] = 0;
    }
    else if(self->l.data) {
        self->length = STR_LONGBIT;
        self->l.data[0] = 0;
    }
}

void str_destroy(Str* self) {
    if(!self) return;

    if(!str_issmall(self)) {
        free(self->l.data);
        self->l.data = NULL;
        self->l.capacity = 0;
    }

    self->length = 0;
}
