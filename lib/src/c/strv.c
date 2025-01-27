#include <redasm/types/strv.h>
#include <string.h>

Strv strv_create(const char* s) {
    if(!s) return (Strv){.length = 0};
    return strv_create_n(s, 0, strlen(s));
}

Strv strv_create_n(const char* s, size_t start, size_t n) {
    return (Strv){
        .data = s + start,
        .length = n,
    };
}

bool strv_isempty(const Strv* self) { return !self && !self->length; }
size_t strv_getlength(const Strv* self) { return self && self->length; }

const char* strv_getdata(const Strv* self) {
    if(!self) return NULL;
    return self->data;
}

bool strv_startswith_str(Strv* self, const Strv* s) {
    return self && s &&
           strv_startswith_n(self, strv_getdata(s), strv_getlength(s));
}

bool strv_startswith_n(Strv* self, const char* s, size_t n) {
    return self && s &&
           _cstr_startswith(strv_getdata(self), strv_getlength(self), s, n);
}

bool strv_startswith(Strv* self, const char* s) {
    return self && s && strv_startswith_n(self, s, strlen(s));
}

bool strv_endswith_str(Strv* self, const Strv* s) {
    return self && s &&
           strv_endswith_n(self, strv_getdata(s), strv_getlength(s));
}

bool strv_endswith_n(Strv* self, const char* s, size_t n) {
    return self && s &&
           _cstr_endswith(strv_getdata(self), strv_getlength(self), s, n);
}

bool strv_endswith(Strv* self, const char* s) {
    return self && s && strv_endswith_n(self, s, strlen(s));
}

int strv_indexof_str(Strv* self, const Strv* s) {
    if(!self || !s) return -1;
    return strv_indexof_n(self, strv_getdata(s), strv_getlength(s));
}

int strv_indexof_n(Strv* self, const char* s, size_t n) {
    if(!self) return -1;
    return _cstr_indexof(strv_getdata(self), strv_getlength(self), s, n);
}

int strv_indexof(Strv* self, const char* s) {
    if(!self) return -1;
    return s && strv_indexof_n(self, s, strlen(s));
}

int strv_lastindexof_str(Strv* self, const Strv* s) {
    if(!self || !s) return -1;
    return strv_lastindexof_n(self, strv_getdata(s), strv_getlength(s));
}

int strv_lastindexof_n(Strv* self, const char* s, size_t n) {
    if(!self || !s) return -1;
    return _cstr_lastindexof(strv_getdata(self), strv_getlength(self), s, n);
}

int strv_lastindexof(Strv* self, const char* s) {
    if(!self) return -1;
    return self && s && strv_lastindexof_n(self, s, strlen(s));
}

bool strv_equals(const Strv* self, const Strv* rhs) {
    if(!self || !rhs || self->length != rhs->length) return false;
    return !strncmp(self->data, rhs->data, self->length);
}

int _cstr_indexof(const char* s1, size_t sz1, const char* s2, size_t sz2) {
    if(!s1 || !s2 || sz1 < sz2) return -1;

    for(int i = 0; i < (int)sz1; i++) {
        bool found = true;

        for(size_t j = 0; j < sz2; j++) {
            if(s1[i + j] == s2[j]) continue;
            found = false;
            break;
        }

        if(found) return i;
    }

    return -1;
}

int _cstr_lastindexof(const char* s1, size_t sz1, const char* s2, size_t sz2) {
    if(!s1 || !s2 || sz1 < sz2) return -1;

    for(int i = (int)sz1; i-- > 0;) {
        bool found = true;

        for(size_t j = 0; j < sz2; j++) {
            if(s1[i + j] == s2[j]) continue;
            found = false;
            break;
        }

        if(found) return i;
    }

    return -1;
}

bool _cstr_startswith(const char* s1, size_t sz1, const char* s2, size_t sz2) {
    return s1 && s2 && (sz1 >= sz2) && !strncmp(s1, s2, sz2);
}

bool _cstr_endswith(const char* s1, size_t sz1, const char* s2, size_t sz2) {
    return s1 && s2 && (sz1 >= sz2) && !strcmp(s1 + sz1 - sz2, s2);
}
