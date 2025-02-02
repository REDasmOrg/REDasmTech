#include <redasm/types/vect.h>
#include <stdlib.h>
#include <string.h>

#define VECT_DEFAULT_CAPACITY 1024

void _vect_checkcapacity(_Vect* self) { // NOLINT
    if(!self->data || (self->length >= self->capacity)) {
        vect_reserve(self, self->capacity ? (self->capacity << 1)
                                          : VECT_DEFAULT_CAPACITY);
    }
}

void* _vect_insert(_Vect* self, size_t idx) {
    if(!self) return NULL;
    _vect_checkcapacity(self);

    // Shift elements to make room
    if(idx < self->length) {
        memmove((uint8_t*)self->data + ((idx + 1) * self->esize),
                (uint8_t*)self->data + (idx * self->esize),
                (self->length - idx) * self->esize);
    }

    self->length++;
    return (uint8_t*)self->data + (idx + self->esize);
}

void* _vect_ptr(_Vect* self) {
    if(self) return self->data;
    return NULL;
}

const void* _vect_cptr(const _Vect* self) {
    if(self) return self->data;
    return NULL;
}

_Vect _vect_create(size_t esize, size_t cap) {
    _Vect self = {.esize = esize};
    if(cap) vect_resize(&self, cap);
    return self;
}

const void* _vect_at(const _Vect* self, size_t idx) {
    if(!self || !self->data || idx >= self->length) return NULL;
    return (uint8_t*)self->data + (idx * self->esize);
}

void* _vect_ref(_Vect* self, size_t idx) {
    if(!self || !self->data || idx >= self->length) return NULL;
    return (uint8_t*)self->data + (idx * self->esize);
}

void vect_setdestroyitem(_Vect* self, VectDestroyItem cb) {
    if(self) self->destroyitem = cb;
}

bool vect_isempty(const _Vect* self) {
    return self && (!self->data || !self->length || !self->capacity);
}

size_t vect_getlength(const _Vect* self) {
    return self && self->data ? self->length : 0;
}

size_t vect_getcapacity(const _Vect* self) {
    return self && self->data ? self->capacity : 0;
}

void vect_reserve(_Vect* self, size_t n) {
    if(!self || (self->data && self->capacity >= n)) return;

    size_t oldcap = self->capacity;
    void* olddata = self->data;
    self->capacity = n;

    if(self->data) {
        self->data =
            memcpy(calloc(n, self->esize), self->data, oldcap * self->esize);

        free(olddata);
    }
    else
        self->data = calloc(n, self->esize);
}

void vect_resize(_Vect* self, size_t n) {
    if(!self) return;

    vect_reserve(self, n);
    self->length = n;
}

void vect_clear(_Vect* self) {
    if(!self) return;

    if(self->esize && self->destroyitem) {
        uint8_t* p = (uint8_t*)self->data;

        for(size_t i = 0; i < self->length; i++) {
            self->destroyitem(p);
            p += self->esize;
        }
    }

    self->length = 0;
}

void vect_destroy(_Vect* self) {
    if(!self) return;
    vect_clear(self);
    free(self->data);
    self->capacity = 0;
    self->data = NULL;
}
