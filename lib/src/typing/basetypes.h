#pragma once

#include "base.h"
#include "tl/optional.hpp"
#include "typedef.h"
#include <fmt/core.h>
#include <redasm/typing.h>
#include <unordered_map>

namespace redasm::typing {

struct ParsedType {
    const TypeDef* tdef; // Cannot be null
    usize n;

    [[nodiscard]] RDType to_type() const { return tdef->to_type(n); }
    [[nodiscard]] bool is_list() const { return n > 0; }
};

struct BaseTypes {
    BaseTypes();
    [[nodiscard]] const TypeDef* get_typedef(RDType t, TypeName h = {}) const;
    [[nodiscard]] ParsedType parse(FullTypeName tn) const;
    [[nodiscard]] TypeName type_name(RDType t) const;
    [[nodiscard]] std::string to_string(ParsedType pt) const;
    [[nodiscard]] std::string to_string(RDType t) const;
    [[nodiscard]] RDType from_string(FullTypeName tn) const;
    [[nodiscard]] tl::optional<RDType> int_from_bytes(usize n,
                                                      bool sign = false) const;

    [[nodiscard]] usize size_of(RDType t) const;
    [[nodiscard]] usize size_of(FullTypeName tn,
                                ParsedType* res = nullptr) const;

    std::string as_array(TypeName tn, usize n) const {
        return fmt::format("{}[{}]", tn, n);
    }

    [[nodiscard]] std::string as_array(RDType t, usize n) const {
        return this->as_array(this->type_name(t), n);
    }

    [[nodiscard]] const TypeDef* get_typedef(TypeName tn) const {
        return this->get_typedef({.id = typing::type_id(tn)}, tn);
    }
    [[nodiscard]] const TypeDef* get_typedef(TypeId tid) const {
        return this->get_typedef({.id = tid});
    }

protected:
    std::unordered_map<TypeId, typing::TypeDef> m_registered;
};

} // namespace redasm::typing
