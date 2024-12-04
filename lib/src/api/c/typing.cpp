#include "../internal/typing.h"
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

const char* rd_createstruct(const char* name, const RDStructField* fields,
                            usize n) {
    if(!name || !fields || !n)
        return nullptr;

    static std::string res;

    redasm::typing::StructBody body;
    body.reserve(n);

    for(usize i = 0; i < n; i++)
        body.emplace_back(fields[i].type, fields[i].name);

    res = redasm::api::internal::create_struct(name, body);
    return res.c_str();
}
