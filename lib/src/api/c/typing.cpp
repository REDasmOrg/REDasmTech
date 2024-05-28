#include "../internal/typing.h"
#include <redasm/typing.h>

usize rd_sizeof(const char* tname) {
    if(!tname)
        return 0;

    return redasm::api::internal::size_of(tname);
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
