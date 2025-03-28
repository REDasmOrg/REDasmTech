#include "../context.h"
#include "../state.h"
#include "../typing/base.h"
#include "../utils/utils.h"
#include <redasm/typing.h>
#include <spdlog/spdlog.h>

const u32 TID_BOOL = redasm::typing::ids::BOOL;
const u32 TID_CHAR = redasm::typing::ids::CHAR;
const u32 TID_WCHAR = redasm::typing::ids::WCHAR;
const u32 TID_U8 = redasm::typing::ids::U8;
const u32 TID_U16 = redasm::typing::ids::U16;
const u32 TID_U32 = redasm::typing::ids::U32;
const u32 TID_U64 = redasm::typing::ids::U64;
const u32 TID_I8 = redasm::typing::ids::I8;
const u32 TID_I16 = redasm::typing::ids::I16;
const u32 TID_I32 = redasm::typing::ids::I32;
const u32 TID_I64 = redasm::typing::ids::I64;
const u32 TID_U16BE = redasm::typing::ids::U16BE;
const u32 TID_U32BE = redasm::typing::ids::U32BE;
const u32 TID_U64BE = redasm::typing::ids::U64BE;
const u32 TID_I16BE = redasm::typing::ids::I16BE;
const u32 TID_I32BE = redasm::typing::ids::I32BE;
const u32 TID_I64BE = redasm::typing::ids::I64BE;
const u32 TID_STR = redasm::typing::ids::STR;
const u32 TID_WSTR = redasm::typing::ids::WSTR;

usize rd_nsizeof(const char* tname) {
    spdlog::trace("rd_nsizeof('{}')", tname);
    return redasm::state::get_types().size_of(tname);
}

usize rd_tsizeof(const RDType* t) {
    spdlog::trace("rd_tsizeof('{}')", fmt::ptr(t));
    if(t) return redasm::state::get_types().size_of(*t);
    return 0;
}

bool rdtype_create_n(const char* tname, usize n, RDType* t) {
    spdlog::trace("rdtype_create_n('{}', {}, {})", tname, n, fmt::ptr(t));
    if(!tname || !t) return false;
    *t = redasm::state::get_types().parse(tname).to_type();
    t->n = n;
    return true;
}

bool rdtype_create(const char* tname, RDType* t) {
    return rdtype_create_n(tname, 0, t);
}

bool rd_intfrombytes(usize b, bool sign, RDType* t) {
    spdlog::trace("rd_intfrombytes({}, {}, {})", b, sign, fmt::ptr(t));
    if(!t) return false;
    auto inttype = redasm::state::get_types().int_from_bytes(b, sign);
    if(inttype) *t = *inttype;
    return inttype.has_value();
}

const char* rd_typename(const RDType* t) {
    static std::string s;

    if(t)
        s = redasm::state::get_types().to_string(*t);
    else
        s.clear();

    if(!s.empty()) return s.c_str();
    return nullptr;
}

const char* rd_createstruct(const char* name, const RDStructField* fields) {
    spdlog::trace("rd_createstruct('{}', {})", name, fmt::ptr(fields));

    if(redasm::state::context) {
        const redasm::typing::TypeDef* td =
            redasm::state::context->types.declare(name, fields);
        if(td) return td->name.c_str();
    }

    return nullptr;
}

void rdvalue_init(RDValue* self) {
    spdlog::trace("rdvalue_create({})", fmt::ptr(self));
    ct_assume(self);

    *self = {};
    slice_init(&self->list, nullptr, nullptr);
    hmap_init(&self->dict, str_hash_cstr);
    str_init(&self->str, nullptr, nullptr);
    str_init(&self->_scratchpad, nullptr, nullptr);
}

void rdvalue_destroy(RDValue* self) {
    spdlog::trace("rdvalue_destroy({})", fmt::ptr(self));
    if(!self) return;

    RDValue* v;
    slice_foreach(v, &self->list) rdvalue_destroy(v);

    RDValueHNode *hv, *tmp;
    hmap_foreach_safe(hv, tmp, &self->dict, RDValueHNode, hnode) {
        delete[] hv->key;
        rdvalue_destroy(&hv->value);
        delete hv;
    }

    slice_destroy(&self->list);
    str_destroy(&self->str);
    str_destroy(&self->_scratchpad);
}

const char* rdvalue_tostring(RDValue* self) {
    spdlog::trace("rdvalue_tostring({})", fmt::ptr(self));

    if(rdvalue_islist(self)) {
        str_resize(&self->_scratchpad, self->list.length);

        for(isize i = 0; i < self->list.length; i++)
            self->_scratchpad.data[i] = self->list.data[i].ch_v;

        return self->_scratchpad.data;
    }

    return self->str.data;
}

bool rdvalue_islist(const RDValue* self) {
    spdlog::trace("rdvalue_islist({})", fmt::ptr(self));
    return self && self->type.n && !slice_empty(&self->list);
}

bool rdvalue_isstruct(const RDValue* self) {
    spdlog::trace("rdvalue_isstruct({})", fmt::ptr(self));
    return self && !hmap_empty(&self->dict);
}

usize rdvalue_getlength(const RDValue* self) {
    spdlog::trace("rdvalue_getlength({})", fmt::ptr(self));
    if(!self) return 0;

    if(rdvalue_islist(self)) return self->list.length;
    if(rdvalue_isstruct(self)) return hmap_length(&self->dict);

    if(self->type.id == redasm::typing::ids::STR ||
       self->type.id == redasm::typing::ids::WSTR)
        return self->str.length;

    ct_exceptf("Cannot get value-length of type '%s'",
               rd_typename(&self->type));
}

const RDValue* rdvalue_query(const RDValue* self, const char* q,
                             const char** error) {
    return rdvalue_query_n(self, q, q ? std::strlen(q) : 0, error);
}

const RDValue* rdvalue_query_n(const RDValue* self, const char* q, usize n,
                               const char** error) {
    auto set_error = [&](const char* msg) {
        if(error) *error = msg;
        return false;
    };

    if(error) *error = nullptr;
    if(!self) return nullptr;

    if(!q) {
        q = ".";
        n = 1;
    }

    if(*q == '.' && *(q + 1) == '\0') return self;

    const RDValue* curr = self;
    const char *kstart = q, *endq = q + n;
    bool inindex = false;

    auto traverse_key = [&](std::string_view key) -> bool {
        if(!rdvalue_isstruct(curr))
            return set_error("Attempted key lookup on non-struct value");

        RDValueHNode* it = nullptr;
        hmap_get(it, &curr->dict, RDValueHNode, hnode, key.data(),
                 it->key == key);

        if(it) {
            curr = &it->value;
            return true;
        }

        return set_error("Key not found.");
    };

    auto traverse_index = [&](isize index) -> bool {
        if(!rdvalue_islist(curr) || index >= curr->list.length)
            return set_error("Index out of bounds");
        curr = &curr->list.data[index];
        return true;
    };

    while(q <= endq) {
        if(*q == '.' || *q == '[' || q == endq) {
            if(!rdvalue_islist(curr)) {
                usize klen = q - kstart;
                if(!klen) {
                    set_error("Invalid key: empty segment detected");
                    return nullptr;
                }

                if(!inindex && !traverse_key(std::string_view{kstart, klen}))
                    return nullptr;
            }

            if(*q == '[') {
                inindex = true;
                kstart = q + 1; // Start parsing the index
            }
            else if(*q == '.' || *q == '\0') {
                kstart = q + 1; // Move to the next key
            }
        }
        else if(*q == ']') {
            if(!inindex) {
                set_error("Unmatched closing bracket ']'");
                return nullptr;
            }

            // Parse the index
            auto index = redasm::utils::to_integer(std::string_view{kstart, q});
            if(!index || !traverse_index(*index)) return nullptr;

            inindex = false;
            if(++q == endq) break; // Skip ']'
            if(*q == '.') q++;     // Skip '.'
            kstart = q;
        }

        q++;
    }

    if(inindex) {
        set_error("Unmatched opening bracket '['");
        return nullptr;
    }

    return curr;
}
