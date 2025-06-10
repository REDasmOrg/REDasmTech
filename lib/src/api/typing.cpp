#include "../context.h"
#include "../memory/memory.h"
#include "../state.h"
#include "../utils/utils.h"
#include <redasm/typing.h>
#include <spdlog/spdlog.h>

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

    if(rdvalue_islist(self)) return slice_length(&self->list);
    if(rdvalue_isstruct(self)) return hmap_length(&self->dict);

    if(self->type.def && self->type.def->kind == TK_PRIMITIVE &&
       (self->type.def->t_primitive == T_STR ||
        self->type.def->t_primitive == T_WSTR))
        return self->str.length;

    ct_exceptf("Cannot get value-length of type '%s'",
               rd_typename(&self->type));
}

bool rdvalue_getunsigned(const RDValue* self, u64* v) {
    spdlog::trace("rdvalue_getunsigned({}, {})", fmt::ptr(self), fmt::ptr(v));

    if(!self || !v || self->type.n > 0 || self->type.def->kind != TK_PRIMITIVE)
        return false;

    switch(self->type.def->t_primitive) {
        case T_U8: *v = self->u8_v; break;
        case T_U16:
        case T_U16BE: *v = self->u16_v; break;
        case T_U32:
        case T_U32BE: *v = self->u32_v; break;
        case T_U64:
        case T_U64BE: *v = self->u64_v; break;
        default: return false;
    }

    return true;
}

bool rdvalue_getsigned(const RDValue* self, i64* v) {
    spdlog::trace("rdvalue_getsigned({}, {})", fmt::ptr(self), fmt::ptr(v));

    if(!self || !v || self->type.n > 0 || self->type.def->kind != TK_PRIMITIVE)
        return false;

    switch(self->type.def->t_primitive) {
        case T_I8: *v = self->i8_v; break;
        case T_I16:
        case T_I16BE: *v = self->i16_v; break;
        case T_I32:
        case T_I32BE: *v = self->i32_v; break;
        case T_I64:
        case T_I64BE: *v = self->i64_v; break;
        default: return false;
    }

    return true;
}

bool rdvalue_getinteger(const RDValue* self, u64* v) {
    if(!self || !v) return false;

    u64 res1;
    i64 res2;

    if(rdvalue_getunsigned(self, &res1)) {
        *v = res1;
        return true;
    }

    if(rdvalue_getsigned(self, &res2)) {
        *v = static_cast<u64>(res2);
        return true;
    }

    return false;
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
        hmap_get(it, &curr->dict, key.data(), RDValueHNode, hnode,
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

usize rd_nsizeof(const char* tname) {
    spdlog::trace("rd_nsizeof('{}')", tname);
    if(tname) return redasm::state::get_types().size_of(tname);
    return 0;
}

usize rd_tsizeof(const RDType* t) {
    spdlog::trace("rd_tsizeof('{}')", fmt::ptr(t));
    if(t) return redasm::state::get_types().size_of(*t);
    return 0;
}

bool rd_createprimitive(RDPrimitiveType pt, RDType* t) {
    return rd_createprimitive_n(pt, 0, t);
}

bool rd_createprimitive_n(RDPrimitiveType pt, usize n, RDType* t) {
    spdlog::trace("rd_createprimitive_n('{}', {}, {})", static_cast<int>(pt), n,
                  fmt::ptr(t));

    if(t) {
        *t = redasm::state::get_types().create_primitive(pt, n);
        return true;
    }

    return false;
}

bool rd_createtype(const char* tname, RDType* t) {
    spdlog::trace("rd_createtype('{}', {})", tname, fmt::ptr(t));

    if(t && tname) {
        auto res = redasm::state::get_types().create_type(tname);
        res.map([&](RDType& arg) { *t = arg; });
        return res.has_value();
    }

    return false;
}

bool rd_createtype_n(const char* tname, usize n, RDType* t) {
    spdlog::trace("rd_createtype_n('{}')", tname);
    if(t && tname) {
        auto res = redasm::state::get_types().create_type(tname, n);
        res.map([&](RDType& arg) { *t = arg; });
        return res.has_value();
    }

    return false;
}

const RDTypeDef* rd_createstruct(const char* name,
                                 const RDStructFieldDecl* fields) {
    spdlog::trace("rd_createstruct('{}', {})", name, fmt::ptr(fields));

    if(redasm::state::context)
        return redasm::state::context->types.create_struct(name, fields);

    return nullptr;
}

const char* rd_typename(const RDType* t) {
    spdlog::trace("rd_typename({})", fmt::ptr(t));
    static std::string s;

    if(t)
        s = redasm::state::get_types().to_string(*t);
    else
        s.clear();

    if(!s.empty()) return s.c_str();
    return nullptr;
}

bool rd_settypename(RDAddress address, const char* tname, RDValue* v) {
    return rd_settypename_ex(address, tname, 0, v);
}

bool rd_settypename_ex(RDAddress address, const char* tname, usize flags,
                       RDValue* v) {
    spdlog::trace("rd_settypename_ex({:x}, '{}', {}, {})", address, tname,
                  flags, fmt::ptr(v));
    if(!redasm::state::context || !tname) return false;

    const RDSegment* seg =
        redasm::state::context->program.find_segment(address);
    if(!seg) return false;

    if(redasm::state::context->set_type(address, tname, flags)) {
        if(v) {
            auto t = redasm::memory::get_type(seg, address, tname);
            ct_assume(t);
            *v = *t;
        }

        return true;
    }

    return false;
}

bool rd_intfrombytes(usize bytes, bool sign, RDType* t) {
    spdlog::trace("rd_intfrombytes({}, {}, {})", bytes, sign, fmt::ptr(t));
    if(!t) return false;
    auto inttype = redasm::state::get_types().int_from_bytes(bytes, sign);
    if(inttype) *t = *inttype;
    return inttype.has_value();
}

bool rd_typeequals(const RDType* t1, const RDType* t2) {
    return t1 && t2 && (t1->n == t2->n) && (t1->def == t2->def);
}
