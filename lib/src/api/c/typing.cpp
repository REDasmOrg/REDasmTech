#include "../internal/typing.h"
#include "../../typing/base.h"
#include "../../utils/utils.h"
#include "../marshal.h"
#include <redasm/typing.h>

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

usize rd_sizeof(const char* tname) {
    if(!tname)
        return 0;

    return redasm::api::internal::size_of(tname);
}

bool rd_createtype(const char* tname, RDType* t) {
    if(!tname)
        return false;

    return redasm::api::internal::create_type(tname, t);
}

const char* rd_createstruct(const char* name, const RDStructField* fields) {
    if(!name || !fields)
        return nullptr;

    static std::string res;
    redasm::typing::StructBody body;

    while(fields->name && fields->type) {
        body.emplace_back(fields->type, fields->name);
        fields++;
    }

    res = redasm::api::internal::create_struct(name, body);
    return res.c_str();
}

RDValue* rdvalue_create() {
    return redasm::api::to_c(redasm::api::internal::create_value());
}

const RDType* rdvalue_gettype(const RDValue* self) {
    if(self)
        return &redasm::api::from_c(self)->type;
    return nullptr;
}

const char* rdvalue_tostring(const RDValue* self) {
    if(self)
        return redasm::api::from_c(self)->str.c_str();

    return nullptr;
}

bool rdvalue_get(const RDValue* self, const char* key, const RDValue** v) {
    if(!self)
        return false;

    const redasm::typing::Value* curr = redasm::api::from_c(self);

    redasm::utils::split_each(key, '/', [&](std::string_view x) {
        if(auto it = curr->dict.find(x); it != curr->dict.end())
            curr = &it->second;
        else
            curr = nullptr;

        return curr != nullptr;
    });

    if(v && curr)
        *v = redasm::api::to_c(curr);
    else if(!curr)
        *v = {};

    return curr != nullptr;
}

bool rdvalue_query(const RDValue* self, const char* q, const RDValue** v,
                   const char** error) {
    if(q)
        return rdvalue_query_n(self, q, std::strlen(q), v, error);
    return false;
}

bool rdvalue_query_n(const RDValue* self, const char* q, usize n,
                     const RDValue** v, const char** error) {
    auto set_error = [&](const char* msg) {
        if(error)
            *error = msg;
        return false;
    };

    if(error)
        *error = nullptr;

    if(!self || !q || !v)
        return false;

    if(*q == '.' && *(q + 1) == '\0') {
        if(v)
            *v = self;
        return true;
    }

    const redasm::typing::Value* curr = redasm::api::from_c(self);
    const char *kstart = q, *endq = q + n;
    bool inindex = false;

    auto traverse_key = [&](std::string_view key) -> bool {
        if(!curr->is_struct())
            return set_error("Attempted key lookup on non-struct value");
        if(auto it = curr->dict.find(key); it != curr->dict.end()) {
            curr = &it->second;
            return true;
        }
        return set_error("Key not found.");
    };

    auto traverse_index = [&](usize index) -> bool {
        if(!curr->is_list() || index >= curr->list.size())
            return set_error("Index out of bounds");

        curr = &curr->list[index];
        return true;
    };

    while(q <= endq) {
        if(*q == '.' || *q == '[' || q == endq) {
            usize klen = q - kstart;
            if(klen == 0)
                return set_error("Invalid key: empty segment detected");

            if(!inindex && !traverse_key(std::string_view{kstart, klen}))
                return false;

            if(*q == '[') {
                inindex = true;
                kstart = q + 1; // Start parsing the index
            }
            else if(*q == '.' || *q == '\0') {
                kstart = q + 1; // Move to the next key
            }
        }
        else if(*q == ']') {
            if(!inindex)
                return set_error("Unmatched closing bracket ']'");

            // Parse the index
            auto index = redasm::utils::to_integer(std::string_view{kstart, q});
            if(!index || !traverse_index(*index))
                return false;

            inindex = false;
            kstart = q + 2; // Skip ']'
        }

        q++;
    }

    if(inindex)
        return set_error("Unmatched opening bracket '['");

    if(v)
        *v = redasm::api::to_c(curr);
    return true;
}

bool rdvalue_getu8(const RDValue* self, const char* q, u8* v) {
    const RDValue* res = nullptr;
    const char* error = nullptr;

    if(rdvalue_query(self, q, &res, &error)) {
        if(v)
            *v = redasm::api::from_c(res)->u8_v;
        return true;
    }
    if(error)
        except("{}", error);

    return false;
}

bool rdvalue_getu16(const RDValue* self, const char* q, u16* v) {
    const RDValue* res = nullptr;
    const char* error = nullptr;

    if(rdvalue_query(self, q, &res, &error)) {
        if(v)
            *v = redasm::api::from_c(res)->u16_v;
        return true;
    }
    if(error)
        except("{}", error);

    return false;
}

bool rdvalue_getu32(const RDValue* self, const char* q, u32* v) {
    const RDValue* res = nullptr;
    const char* error = nullptr;

    if(rdvalue_query(self, q, &res, &error)) {
        if(v)
            *v = redasm::api::from_c(res)->u32_v;
        return true;
    }
    if(error)
        except("{}", error);

    return false;
}

bool rdvalue_getu64(const RDValue* self, const char* q, u64* v) {
    const RDValue* res = nullptr;
    const char* error = nullptr;

    if(rdvalue_query(self, q, &res, &error)) {
        if(v)
            *v = redasm::api::from_c(res)->u64_v;
        return true;
    }
    if(error)
        except("{}", error);

    return false;
}

bool rdvalue_geti8(const RDValue* self, const char* q, i8* v) {
    const RDValue* res = nullptr;
    const char* error = nullptr;

    if(rdvalue_query(self, q, &res, &error)) {
        if(v)
            *v = redasm::api::from_c(res)->i8_v;
        return true;
    }
    if(error)
        except("{}", error);

    return false;
}

bool rdvalue_geti16(const RDValue* self, const char* q, i16* v) {
    const RDValue* res = nullptr;
    const char* error = nullptr;

    if(rdvalue_query(self, q, &res, &error)) {
        if(v)
            *v = redasm::api::from_c(res)->i16_v;
        return true;
    }
    if(error)
        except("{}", error);

    return false;
}

bool rdvalue_geti32(const RDValue* self, const char* q, i32* v) {
    const RDValue* res = nullptr;
    const char* error = nullptr;

    if(rdvalue_query(self, q, &res, &error)) {
        if(v)
            *v = redasm::api::from_c(res)->i32_v;
        return true;
    }
    if(error)
        except("{}", error);

    return false;
}

bool rdvalue_geti64(const RDValue* self, const char* q, i64* v) {
    const RDValue* res = nullptr;
    const char* error = nullptr;

    if(rdvalue_query(self, q, &res, &error)) {
        if(v)
            *v = redasm::api::from_c(res)->i64_v;
        return true;
    }
    if(error)
        except("{}", error);

    return false;
}

bool rdvalue_getstr(const RDValue* self, const char* q, const char** v) {
    const RDValue* res = nullptr;
    const char* error = nullptr;

    if(rdvalue_query(self, q, &res, &error)) {
        const redasm::typing::Value* r = redasm::api::from_c(res);

        if(r->is_list()) { // Try to build a string for char array
            r->scratchpad.resize(r->list.size() + 1);
            for(usize i = 0; i < r->list.size(); i++)
                r->scratchpad[i] = r->list[i].ch_v;
            r->scratchpad.back() = '\0';

            if(v)
                *v = r->scratchpad.data();
        }
        else if(v)
            *v = r->str.c_str();

        return true;
    }
    if(error)
        except("{}", error);

    return false;
}
