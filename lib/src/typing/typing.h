#pragma once

#include <map>
#include <redasm/typing.h>
#include <string>
#include <string_view>
#include <tl/optional.hpp>

namespace redasm::typing {

struct BaseTypes {
    BaseTypes();
    ~BaseTypes();
    tl::optional<RDType> int_from_bytes(usize sz, bool sign = false) const;
    RDType create_primitive(RDPrimitiveType pt, usize n = 0) const;
    tl::optional<RDType> create_type(std::string_view tname, usize n) const;
    tl::optional<RDType> create_type(std::string_view tname) const;
    tl::optional<RDType> create_type(const RDTypeDef* tdef,
                                        usize n = 0) const;

    usize size_of(std::string_view tname) const;
    usize size_of(RDType t) const;
    void destroy_type(RDTypeDef* t);

    const RDTypeDef* get_primitive(RDPrimitiveType pt) const;
    const RDTypeDef* get_type(std::string_view name) const;
    std::string as_array(RDPrimitiveType pt, usize n) const;
    std::string as_array(std::string_view name, usize n) const;
    std::string to_string(RDType t) const;

protected:
    std::map<std::string, RDTypeDef*, std::less<>> m_registered;
};

struct Types: public BaseTypes {
    RDTypeDef* create_struct(const std::string& name,
                                const RDStructFieldDecl* fields);
};

} // namespace redasm::typing
