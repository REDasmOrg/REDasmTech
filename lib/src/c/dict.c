#include <redasm/types/dict.h>
#include <stdlib.h>
#include <string.h>

#if UINTPTR_MAX == 0xFFFFFFFF                                 // 32-bit platform
static uint32_t _dict_fnv1a(const void* data, uint32_t len) { // NOLINT
    const uint8_t* p = (const uint8_t*)(data);
    uint32_t hash = 2166136261U;

    for(uint32_t i = 0; i < len; i++) {
        hash ^= (uint32_t)(p[i]);
        hash *= 16777619;
    }

    return hash;
}
#elif UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF // 64-bit platform
static uint64_t _dict_fnv1a(const void* data, uint64_t len) { // NOLINT
    const uint8_t* p = (const uint8_t*)(data);
    uint64_t hash = 14695981039346656037ULL;

    for(uint64_t i = 0; i < len; i++) {
        hash ^= (uint64_t)(p[i]);
        hash *= 1099511628211;
    }

    return hash;
}
#else
#error "Unknown platform"
#endif

#define DICT_DEFAULTLOADFACTOR 0.75f
#define DICT_INITIALCAPACITY 1031 // Prime Number

static bool _dict_defaultequal(const void* key1, // NOLINT
                               const void* key2, size_t n) {
    return !memcmp(key1, key2, n);
}

static bool _dict_defaultequalstr(const void* key1, // NOLINT
                                  const void* key2, size_t n) {
    (void)n;
    return !strcmp((const char*)key1, (const char*)key2);
}

static size_t _dict_defaulthash(const void* key, size_t n) { // NOLINT
    return _dict_fnv1a(key, n);
}

static size_t _dict_defaulthashstr(const void* key, size_t n) { // NOLINT
    (void)n;
    return _dict_fnv1a(key, strlen((const char*)key));
}

static void* _dict_getnodekey(_Dict* self, // NOLINT
                              DictNode* n) {
    if(self->ksize == -1U) return *(char**)((uint8_t*)n + sizeof(DictNode*));
    return (uint8_t*)n + sizeof(DictNode*);
}

static void* _dict_getnodevalue(_Dict* self, // NOLINT
                                DictNode* n) {
    return (uint8_t*)n + self->voff;
}

static bool _dict_isprime(size_t n) { // NOLINT
    if(n == 2 || n == 3) return true;
    if(n % 2 == 0 || n % 3 == 0) return false;

    size_t d = 6;

    while((d * d - 2 * d + 1) <= n) {
        if(n % (d - 1) == 0) return false;
        if(n % (d + 1) == 0) return false;
        d += 6;
    }

    return true;
}

static size_t _dict_nextprime(size_t number) { // NOLINT
    while(!_dict_isprime(number))
        number++;
    return number;
}

static void _dict_freenode(_Dict* self, DictNode* n) { // NOLINT
    if(self->destroyvalue) self->destroyvalue(_dict_getnodevalue(self, n));
    if(self->destroykey) self->destroykey(_dict_getnodekey(self, n));
    free(n);
}

static bool _dict_insertnode(_Dict* self, DictNode* n) { // NOLINT
    const void* key = _dict_getnodekey(self, n);
    size_t idx = self->hash(key, self->ksize) % self->capacity;
    n->next = NULL;

    if(self->buckets[idx].next) {
        DictNode **ncurr = &self->buckets[idx].next, **nprev = NULL;

        while(*ncurr) {
            const void* currkey = _dict_getnodekey(self, *ncurr);

            if(self->equals(key, currkey, self->ksize)) {
                if(nprev) (*nprev)->next = n;
                n->next = (*ncurr)->next;
                _dict_freenode(self, *ncurr);
                *ncurr = n;
                return false;
            }

            nprev = ncurr;
            ncurr = &(*ncurr)->next;
        }

        (*ncurr)->next = n;
    }
    else
        self->buckets[idx].next = n;

    return true;
}

static void _dict_rehash(_Dict* self) { // NOLINT
    size_t oldcapacity = self->capacity;
    DictNode* oldbuckets = self->buckets;

    self->capacity = _dict_nextprime(self->capacity << 1);
    self->buckets = (DictNode*)calloc(self->capacity, self->nsize);

    for(size_t i = 0; i < oldcapacity; i++) {
        DictNode* n = oldbuckets[i].next;

        while(n) {
            DictNode* next = n->next;
            _dict_insertnode(self, n);
            n = next;
        }
    }

    free(oldbuckets);
}

static void _dict_checkcapacity(_Dict* self) { // NOLINT
    if(!self->capacity) {
        self->capacity = DICT_INITIALCAPACITY;
        self->buckets = (DictNode*)calloc(self->capacity, self->nsize);
        return;
    }

    if(dict_getloadfactor(self) > DICT_DEFAULTLOADFACTOR) _dict_rehash(self);
}

_Dict _dict_create(uint32_t nsize, uint32_t ksize, uint32_t voff, bool str) {
    return (_Dict){
        .capacity = 0,
        .length = 0,
        .nsize = nsize,
        .ksize = str ? -1U : ksize,
        .voff = voff,
        .buckets = NULL,
        .equals = str ? _dict_defaultequalstr : _dict_defaultequal,
        .hash = str ? _dict_defaulthashstr : _dict_defaulthash,
    };
}

DictNode* _dict_createnode(const _Dict* self) {
    if(!self) return NULL;
    return (DictNode*)calloc(1, self->nsize);
}

void dict_setequals(_Dict* self, _DictEquals cb) { self->equals = cb; }
void dict_sethash(_Dict* self, _DictHash cb) { self->hash = cb; }

void dict_setdestroykey(_Dict* self, _DictDestroy cb) {
    if(self) self->destroykey = cb;
}

void dict_setdestroyvalue(_Dict* self, _DictDestroy cb) {
    if(self) self->destroyvalue = cb;
}

bool dict_isempty(const _Dict* self) {
    return self && (!self->buckets || !self->length || !self->capacity);
}

size_t dict_getlength(const _Dict* self) {
    return self && self->buckets ? self->length : 0;
}

size_t dict_getcapacity(const _Dict* self) {
    return self && self->buckets ? self->capacity : 0;
}

float dict_getloadfactor(const _Dict* self) {
    if(!self) return 0;
    return (float)self->length / (float)self->capacity;
}

const DictNode* _dict_get(const _Dict* self, const void* key) { // NOLINT
    if(!self) return NULL;

    size_t idx = self->hash(key, self->ksize) % self->capacity;
    DictNode* ncurr = self->buckets[idx].next;

    while(ncurr) {
        const void* currkey = _dict_getnodekey((_Dict*)self, ncurr);
        if(self->equals(key, currkey, self->ksize)) return ncurr;
        ncurr = ncurr->next;
    }

    return NULL;
}

void _dict_set(_Dict* self, DictNode* n) {
    if(!self) return;

    _dict_checkcapacity(self);
    if(_dict_insertnode(self, n)) self->length++;
}

void dict_clear(_Dict* self) {
    if(!self) return;

    for(size_t i = 0; i < self->capacity; i++) {
        DictNode* n = self->buckets[i].next;

        while(n) {
            DictNode* next = n->next;
            _dict_freenode(self, n);
            n = next;
        }

        self->buckets[i].next = NULL;
    }

    self->length = 0;
}

void dict_destroy(_Dict* self) {
    if(!self) return;

    dict_clear(self);
    free(self->buckets);
    self->capacity = self->length = 0;
    self->buckets = NULL;
}
