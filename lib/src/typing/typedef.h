#pragma once

#include "../utils/hash.h"
#include <redasm/typing.h>
#include <string>
#include <vector>

namespace redasm::typing {

enum TypeDefFlags {
    TD_NONE = 0,
    TD_BIG = (1 << 0),
    TD_VAR = (1 << 1),
    TD_INT = (1 << 2),
    TD_SIGNED = (1 << 3),
};

struct TypeDef {
    std::string name;
    usize size{};
    usize flags;
    std::vector<std::pair<RDType, std::string>> dict;

    TypeId get_id() const { return hash::murmur3(name); }
    bool is_struct() const { return !dict.empty(); }
    bool is_var() const { return flags & TD_VAR; }
    bool is_big() const { return flags & TD_BIG; }
    bool is_int() const { return flags & TD_INT; }
    bool is_signed() const { return flags & TD_SIGNED; }
    RDType to_type(usize n = 0) const { return {.id = this->get_id(), .n = n}; }
};

} // namespace redasm::typing
