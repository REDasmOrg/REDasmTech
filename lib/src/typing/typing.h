#pragma once

#include "../utils/hash.h"
#include "typedef.h"
#include "value.h"
#include <fmt/core.h>
#include <forward_list>
#include <redasm/typing.h>
#include <string_view>
#include <tl/optional.hpp>
#include <unordered_map>

namespace redasm::typing {

using FullTypeName = std::string_view;
using TypeName = std::string_view;
using StructField = std::pair<std::string, std::string>;
using StructBody = std::vector<typing::StructField>;
using StructField = std::pair<std::string, std::string>;
using StructBody = std::vector<typing::StructField>;

struct ParsedType {
    const TypeDef* type; // Cannot be null
    usize n;

    [[nodiscard]] RDType to_type() const { return type->to_type(n); }
    [[nodiscard]] bool is_list() const { return n > 0; }
};

struct Types {
    Types();
    const TypeDef* declare(const std::string& name, const StructBody& arg);
    [[nodiscard]] const TypeDef* get_typedef(RDType t, TypeName h = {}) const;
    [[nodiscard]] ParsedType parse(FullTypeName tn) const;
    [[nodiscard]] TypeName type_name(RDType t) const;
    [[nodiscard]] std::string to_string(ParsedType pt) const;
    [[nodiscard]] std::string to_string(RDType t) const;
    [[nodiscard]] RDType from_string(FullTypeName tn) const;
    [[nodiscard]] RDType int_from_bytes(usize n, bool sign = false) const;
    [[nodiscard]] usize size_of(RDType t) const;

    [[nodiscard]] usize size_of(FullTypeName tn,
                                ParsedType* res = nullptr) const;

    [[nodiscard]] TypeId type_id(TypeName tn) const {
        return hash::murmur3(tn);
    }

    [[nodiscard]] std::string as_array(TypeName tn, usize n) const {
        return fmt::format("{}[{}]", tn, n);
    }

    [[nodiscard]] std::string as_array(RDType t, usize n) const {
        return this->as_array(this->type_name(t), n);
    }

    [[nodiscard]] const TypeDef* get_typedef(TypeName tn) const {
        return this->get_typedef({.id = this->type_id(tn)}, tn);
    }

    std::unordered_map<TypeId, typing::TypeDef> registered;
    std::forward_list<Value> valuespool;
};

} // namespace redasm::typing
