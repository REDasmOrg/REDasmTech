#include "../context.h"
#include "../error.h"
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

RDValue* rdvalue_create() {
    spdlog::trace("rdvalue_create()");
    return new RDValue{};
}

void rdvalue_destroy(RDValue* self) {
    spdlog::trace("rdvalue_destroy({})", fmt::ptr(self));
    if(!self) return;

    vect_foreach(RDValue*, v, self->list) rdvalue_destroy(*v);

    map_foreach(RDValueField, f, self->dict) {
        delete[] f->key;
        rdvalue_destroy(f->value);
    }

    vect_destroy(self->list);
    map_destroy(self->dict);
    str_destroy(self->str);
    str_destroy(self->_scratchpad);
    delete self;
}

const char* rdvalue_tostring(RDValue* self) {
    spdlog::trace("rdvalue_tostring({})", fmt::ptr(self));

    if(rdvalue_islist(self)) {
        if(self->_scratchpad)
            str_resize(self->_scratchpad, vect_length(self->list));
        else
            self->_scratchpad = str_create_n(vect_length(self->list));

        for(usize i = 0; i < vect_length(self->list); i++)
            self->_scratchpad[i] = self->list[i]->ch_v;

        return self->_scratchpad;
    }

    return self->str;
}

bool rdvalue_islist(const RDValue* self) {
    spdlog::trace("rdvalue_islist({})", fmt::ptr(self));
    return self && self->type.n && !vect_empty(self->list); // NOLINT
}

bool rdvalue_isstruct(const RDValue* self) {
    spdlog::trace("rdvalue_isstruct({})", fmt::ptr(self));
    return self && !map_empty(self->dict); // NOLINT
}

usize rdvalue_getlength(const RDValue* self) {
    spdlog::trace("rdvalue_getlength({})", fmt::ptr(self));
    if(!self) return 0;

    if(rdvalue_islist(self)) return vect_length(self->list);
    if(rdvalue_isstruct(self)) return map_length(self->dict);

    if(self->type.id == redasm::typing::ids::STR ||
       self->type.id == redasm::typing::ids::WSTR)
        return str_length(self->str);

    except("Cannot get value-length of type '{}'", rd_typename(&self->type));
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
        if(const auto* f = map_get(RDValueField, curr->dict, key.data()); f) {
            curr = *f;
            return true;
        }
        return set_error("Key not found.");
    };

    auto traverse_index = [&](usize index) -> bool {
        if(!rdvalue_islist(curr) || index >= vect_length(curr->list))
            return set_error("Index out of bounds");
        curr = curr->list[index];
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
