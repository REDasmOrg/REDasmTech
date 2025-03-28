#include <limits.h>
#include <redasm/ct.h>
#include <stdarg.h>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

static inline isize calculate_padding(isize n) {
    return -n & (sizeof(max_align_t) - 1);
}

usize _str_hash_n(const char* s, isize n) {
    ct_assume(s);
    usize h = 5381;

    while(n-- > 0)
        h = ((h << 5) + h) + (u8)*s++;

    return h;
}

void _except_impl(const char* file, int line, const char* fmt, ...) {
    va_list args;
    fprintf(stderr, "[%s:%d] ", file, line);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    abort();
}

LinearArena lineararena_create(isize n) {
    return lineararena_createfrom_n(malloc(n), n);
}

LinearArena lineararena_createfrom_n(char* p, isize n) {
    return (LinearArena){
        .begin = p,
        .end = p ? p + n : p,
    };
}

void lineararena_reset(LinearArena* self) {
    ct_assume(self);
    self->end = self->begin;
}

void lineararena_destroy(LinearArena* self) {
    ct_assume(self);
    free(self->begin);
    self->begin = self->end = NULL;
}

void* std_allocator(void* ctx, void* ptr, isize osize, isize nsize) {
    (void)ctx;

    if(nsize == 0) {
        free(ptr);
        return NULL;
    }

    void* p = malloc(nsize);

    if(!p) {
        abort();
        return NULL;
    }

    if(ptr) {
        memcpy(p, ptr, osize);
        free(ptr);
    }

    return p;
}

void* linear_allocator(void* ctx, void* ptr, isize osize, isize nsize) {
    LinearArena* l = (LinearArena*)ctx;

    if(nsize == 0) {
        if(ptr == l->end) l->end += osize;
        return NULL;
    }

    isize avail = l->end - l->begin;
    isize pad = calculate_padding(nsize);

    if(nsize > avail - pad) {
        abort();
        return NULL;
    }

    char* p = l->end - (nsize + pad);
    if(ptr == l->end)
        memmove(p, l->end, osize);
    else
        memcpy(p, ptr, osize);
    return l->end = p;
}

void* mem_alloc(isize n, Allocator a, void* ctx) {
    ct_assume(a);
    return a(ctx, NULL, 0, n);
}

void* mem_alloc0(isize n, Allocator a, void* ctx) {
    ct_assume(a);
    void* p = mem_alloc(n, a, ctx);
    if(p) memset(p, 0, n);
    return p;
}

void* mem_realloc(void* ptr, isize osize, isize nsize, Allocator a, void* ctx) {
    ct_assume(a);
    return a(ctx, ptr, osize, nsize);
}

void* mem_realloc0(void* ptr, isize osize, isize nsize, Allocator a,
                   void* ctx) {
    ct_assume(a);
    void* p = mem_realloc(ptr, osize, nsize, a, ctx);
    if(p) memset((char*)p + osize, 0, nsize - osize);
    return p;
}

void mem_free(void* ptr, isize n, Allocator a, void* ctx) {
    if(ptr) a(ctx, ptr, n, 0);
}

#define slice_defaultcapacity 256
define_slice(InternalReplicaSlice, void);

void slice_init(void* self, Allocator a, void* ctx) {
    ct_assume(self);
    InternalReplicaSlice* slice = (InternalReplicaSlice*)self;
    slice->data = NULL;
    slice->length = 0;
    slice->capacity = 0;
    slice->alloc = a ? a : std_allocator;
    slice->ctx = ctx ? ctx : NULL;
}

void _slice_insert(void* self, isize idx, isize dsize) {
    ct_assume(self);
    InternalReplicaSlice* slice = (InternalReplicaSlice*)self;

    // Normalize index: -1 or length means "append"
    if(idx == -1) idx = slice->length;

    ct_except_if(idx < 0 || idx > slice->length,
                 

    _slice_grow(self, dsize);

    if(idx < slice->length) {
        void* dst = (char*)slice->data + ((idx + 1) * dsize);
        void* src = (char*)slice->data + (idx * dsize);
        isize n = (slice->length - idx) * dsize;
        memmove(dst, src, n);
    }
}

void _slice_grow(void* self, isize dsize) {
    ct_assume(self);
    InternalReplicaSlice* slice = (InternalReplicaSlice*)self;

    if(slice->length >= slice->capacity) {
        _slice_reserve(self,
                       slice->capacity ? slice->capacity << 1
                                       : slice_defaultcapacity,
                       dsize);
    }
}

void _slice_reserve(void* self, isize n, isize dsize) {
    ct_assume(self);
    InternalReplicaSlice* slice = (InternalReplicaSlice*)self;
    if(slice->capacity >= n) return;

    slice->data = mem_realloc0(slice->data, slice->capacity * dsize, n * dsize,
                               slice->alloc, slice->ctx);
    slice->capacity = n;
}

void _slice_destroy(void* self, isize dsize) {
    ct_assume(self);
    InternalReplicaSlice* slice = (InternalReplicaSlice*)self;
    if(!slice->data) return;

    mem_free(slice->data, slice->capacity * dsize, slice->alloc, slice->ctx);
    slice->length = slice->capacity = 0;
    slice->alloc = NULL;
    slice->data = NULL;
    slice->ctx = NULL;
}

BSearchResult _slice_bsearch(const void* self, isize dsize, const void* key,
                             SliceCompare cmp) {
    ct_assume(self);
    InternalReplicaSlice* slice = (InternalReplicaSlice*)self;
    if(!slice->data) return (BSearchResult){false, -1};

    isize low = 0, high = slice->length;
    const char* base = (const char*)slice->data;

    while(low < high) {
        isize mid = low + ((high - low) / 2);
        int result = cmp(key, base + (mid * dsize));

        if(result == 0) return (BSearchResult){.found = true, .index = mid};

        if(result < 0)
            high = mid;
        else
            low = mid + 1;
    }

    return (BSearchResult){.found = false, .index = low};
}

void str_ncat(Str* self, const char* s, isize n) {
    ct_assume(self);
    ct_except_if(str_isview(self), "str_ncat: cannot mutate a view");
    if(!s || !n) return;

    isize req = self->length + n;

    if(req > self->capacity) {
        isize c = self->capacity ? self->capacity : slice_defaultcapacity;
        while(c <= req)
            c <<= 1;
        str_reserve(self, c + 1);
    }

    memcpy(self->data + self->length, s, n);
    self->length += n;
    self->data[self->length] = 0;
}

void str_resize(Str* self, isize n) {
    str_reserve(self, n);
    self->length = n;
}

void str_reserve(Str* self, isize n) {
    ct_assume(self);
    ct_except_if(str_isview(self), "str_reserve: cannot mutate a view");
    slice_reserve(self, n + 1);
}

bool str_equals(const Str* self, const Str* rhs) {
    return self && rhs && self->length == rhs->length &&
           (!self->length || !memcmp(self->data, rhs->data, self->length));
}

void str_deleterange(Str* self, isize start, isize end) {
    ct_assume(self);
    ct_except_if(str_isview(self), "str_deleterange: cannot mutate a view");

    isize len = self->length;
    if(start < 0) start += len;
    if(end < 0) end += len;

    if(start < 0) start = 0;
    if(end > len) end = len;
    if(start >= end) return;

    isize n = end - start;
    memmove(self->data + start, self->data + end, self->length - end);
    self->length -= n;
    self->data[self->length] = 0;
}

void str_tolower(Str* self) {
    ct_assume(self);
    ct_except_if(str_isview(self), "str_tolower: cannot mutate a view");

    for(isize i = 0; i < self->length; ++i) {
        if(self->data[i] >= 'A' && self->data[i] <= 'Z') {
            self->data[i] += 0x20;
        }
    }
}

void str_toupper(Str* self) {
    ct_assume(self);
    ct_except_if(str_isview(self), "str_toupper: cannot mutate a view");

    for(isize i = 0; i < self->length; ++i) {
        if(self->data[i] >= 'a' && self->data[i] <= 'z') {
            self->data[i] -= 0x20;
        }
    }
}

void str_trim(Str* self) {
    ct_assume(self);
    ct_except_if(str_isview(self), "str_trim: cannot mutate a view");

    isize start = 0, end = self->length;
    while(start < end && self->data[start] <= ' ')
        start++;
    while(end > start && self->data[end - 1] <= ' ')
        end--;

    if(start > 0 || end < self->length) {
        isize n = end - start;
        memmove(self->data, self->data + start, n);
        self->length = n;
        self->data[n] = 0;
    }
}

void _str_replace_n(Str* self, const char* from, isize nfrom, const char* to,
                    isize nto) {
    ct_assume(self && from && to);
    ct_except_if(str_isview(self), "str_replace_n: cannot mutate a view");

    if(!nfrom || self->length < nfrom) return;

    isize i = 0;
    while((i = str_indexof(self, from)) != -1) {
        str_deleterange(self, i, i + nfrom);
        _str_insert_n(self, i, to, nto);
    }
}

void str_destroy(Str* self) {
    ct_assume(self);

    // Check if 'Str' is owned
    if(self->alloc && self->capacity) {
        _slice_destroy(self, sizeof(*self->data));
        return;
    }

    self->length = self->capacity = 0;
    self->alloc = NULL;
    self->data = NULL;
    self->ctx = NULL;
}

bool _str_startswith_n(const Str* self, const char* s, isize n) {
    return self && s && n <= self->length && !memcmp(self->data, s, n);
}

bool _str_endswith_n(const Str* self, const char* s, isize n) {
    return self && s && n <= self->length &&
           !memcmp(self->data + self->length - n, s, n);
}

isize _str_indexof_n(const Str* self, const char* s, isize n) {
    if(!self || !s || !n || n > self->length) return -1;

    for(isize i = 0; i <= self->length - n; i++) {
        if(!memcmp(self->data + i, s, n)) return i;
    }

    return -1;
}

isize _str_lastindexof_n(const Str* self, const char* s, isize n) {
    if(!self || !s || !n || n > self->length) return -1;

    for(isize i = self->length - n; i >= 0; i--) {
        if(!memcmp(self->data + i, s, n)) return i;
    }

    return -1;
}

void _str_insert_n(Str* self, isize idx, const char* s, isize n) {
    ct_assume(self && s);
    ct_except_if(str_isview(self), "_str_insert_n: cannot mutate a view");
    if(!n) return;

    isize len = self->length;
    if(idx < 0) idx += len;
    if(idx < 0) idx = 0;
    if(idx > len) idx = len;

    isize req = self->length + len;
    if(req + 1 > self->capacity) {
        isize cap = self->capacity ? self->capacity : slice_defaultcapacity;
        while(cap <= req)
            cap <<= 1;
        str_reserve(self, cap + 1); // +1 for null terminator
    }

    // Shift existing characters
    memmove(self->data + idx + len, self->data + idx, self->length - idx);

    // Copy new content
    memcpy(self->data + idx, s, len);
    self->length += len;
    self->data[self->length] = 0;
}

Str str_create_n(const char* s, isize n, Allocator a, void* ctx) {
    Str self;
    str_init(&self, a, ctx);
    str_ncat(&self, s, n);
    return self;
}

Str str_view_n(const char* s, isize n) {
    return (Str){
        .length = n,
        .capacity = 0,
        .data = (char*)s,
        .ctx = NULL,
        .alloc = NULL,
    };
}

Str str_sub(const Str* self, isize start, isize end) {
    ct_assume(self);
    isize len = self->length;

    // Normalize negative indices
    if(start < 0) start += len;
    if(end < 0) end += len;

    // Clamp to bounds
    if(start < 0) start = 0;
    if(end > len) end = len;
    if(start > end) start = end;

    return (Str){
        .length = end - start,
        .capacity = 0,
        .data = self->data + start,
        .ctx = NULL,
        .alloc = NULL,
    };
}

Str str_dup(const Str* self) {
    ct_assume(self);
    if(str_isview(self)) return *self;

    Str copy = {
        .length = self->length,
        .capacity = self->length,
        .alloc = self->alloc,
        .ctx = self->ctx,
    };

    if(copy.length > 0 && copy.alloc) {
        copy.data = copy.alloc(copy.ctx, NULL, 0, copy.length + 1);
        if(copy.data) {
            memcpy(copy.data, self->data, copy.length);
            copy.data[copy.length] = 0;
        }
        else { // fallback: make it safe if alloc failed
            copy.length = 0;
            copy.capacity = 0;
            copy.data = NULL;
        }
    }
    else
        copy.data = NULL;

    return copy;
}

Str* str_dup_to(const Str* self, Str* dest) {
    ct_assume(self && dest);
    ct_except_if(str_isview(dest), "str_dup_to: cannot mutate a view");

    if(self->length > dest->capacity) {
        isize cap = dest->capacity ? dest->capacity : slice_defaultcapacity;
        while(cap < self->length)
            cap <<= 1;
        str_reserve(dest, cap + 1);
    }

    memcpy(dest->data, self->data, self->length);
    dest->length = self->length;
    dest->data[dest->length] = 0;
    return dest;
}

void str_clear(Str* self) {
    ct_assume(self);
    ct_except_if(str_isview(self), "str_clear: cannot mutate a view");

    if((self)->data) *(self)->data = 0;
    slice_clear(self);
}

void list_push_tail(List* self, ListNode* n) {
    ct_assume(self);
    ct_assume(n);

    if(self->tail) {
        self->tail->next = n;
        n->prev = self->tail;
        n->next = NULL;
        self->tail = n;
    }
    else {
        ct_assume(!self->head);
        self->head = self->tail = n;
        n->prev = n->next = NULL;
    }
}

StrSplit _strsplit_create_n(const Str* self, const char* sep, isize n) {
    return (StrSplit){
        .source = self,
        .pos = 0,
        .sep = sep,
        .nsep = n,
    };
}

Str str_split_next(StrSplit* it) {
    const Str* s = it->source;
    isize pos = it->pos;
    isize len = s->length;
    if(pos >= len) return (Str){0};

    // Scan for the next delimiter
    for(isize i = pos; i + it->nsep <= len; ++i) {
        if(memcmp(s->data + i, it->sep, it->nsep) == 0) {
            // Found delimiter at i
            isize plen = i - pos;
            it->pos = i + it->nsep;

            return (Str){
                .length = plen,
                .capacity = 0,
                .data = s->data + pos,
                .alloc = NULL,
                .ctx = NULL,
            };
        }
    }

    // No delimiter found -> return the rest
    Str last = {
        .data = s->data + pos,
        .length = len - pos,
        .capacity = 0,
        .alloc = NULL,
        .ctx = NULL,
    };

    it->pos = len + 1; // advance past end
    return last;
}

void list_push(List* self, ListNode* n) {
    ct_assume(self);
    ct_assume(n);

    if(self->head) {
        self->head->prev = n;
        n->next = self->head;
        n->prev = NULL;
        self->head = n;
    }
    else {
        ct_assume(!self->tail);
        self->head = self->tail = n;
        n->prev = n->next = NULL;
    }
}

void list_init(List* self) {
    ct_assume(self);
    self->head = self->tail = NULL;
}

void list_del(List* self, ListNode* n) {
    ct_assume(self);
    ct_assume(n);

    if(n->next) n->next->prev = n->prev;
    if(n->prev) n->prev->next = n->next;
    if(self->head == n) self->head = n->next;
    if(self->tail == n) self->tail = n->prev;
    n->prev = n->next = NULL;
}

void hlist_push(HList* self, HListNode* n) {
    n->next = self->first;
    if(n->next) n->next->backlink = &n->next;
    self->first = n;
    n->backlink = &self->first;
}

void hlist_del(HListNode* n) {
    if(n->next) n->next->backlink = n->backlink;
    *n->backlink = n->next;
}

void _hmap_init(HList* self, usize n) {
    ct_assume(self);
    for(usize i = 0; i < n; i++)
        self[i].first = NULL;
}

uptr _hmap_directhash(const void* p) { return (uptr)p; }

unsigned long _hmap_bits(unsigned long cap) {
    if(!cap) return sizeof(unsigned long) * CHAR_BIT;

#if defined(__GNUC__) || defined(__clang__)
#if UINTPTR_MAX == 0xFFFFFFFF
    return 31 - __builtin_clz(cap);
#elif UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF
    return 63 - __builtin_clzl(cap);
#else
#error "Unsupported platform bits"
#endif
#elif defined(_MSC_VER)
    unsigned long lz;
#if UINTPTR_MAX == 0xFFFFFFFF
    if(_BitScanReverse(&lz, cap)) return lz;
#elif UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF
    if(_BitScanReverse64(&lz, cap)) return lz;
#else
#error "Unsupported platform bits"
#endif
    return sizeof(unsigned long) * CHAR_BIT;
#else
#error "No CLZ intrinsic found"
#endif
}
