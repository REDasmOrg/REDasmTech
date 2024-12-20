#pragma once

#include "../utils/object.h"
#include <map>
#include <redasm/types.h>
#include <redasm/typing.h>
#include <string>
#include <vector>

namespace redasm::typing {

class Value;
using ValueList = std::vector<Value>;
using ValueDict = std::map<std::string, Value, std::less<>>;

class Value: public Object {
public:
    RDType type;

    ValueList list;
    ValueDict dict;
    std::string str;

    mutable std::vector<char> scratchpad;

    // clang-format off
    union {
        bool b_v; char ch_v;
        u8 u8_v; u16 u16_v; u32 u32_v; u64 u64_v;
        i8 i8_v; i16 i16_v; i32 i32_v; i64 i64_v;
    };
    // clang-format on

    bool is_list() const { return type.n && !list.empty(); }
    bool is_struct() const { return !dict.empty(); }
};

} // namespace redasm::typing
