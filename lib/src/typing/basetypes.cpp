#include "basetypes.h"
#include "../utils/utils.h"
#include <cctype>

namespace redasm::typing {

namespace {

void parse_string(FullTypeName tn, std::string_view& name, usize& n) {
    if(tn.empty()) {
        ct_exceptf("typing::parse('%.*s'): type is empty",
                   static_cast<int>(tn.size()), tn.data());
    }

    usize idx = 0;
    std::string_view strn;

    // Ignore leading whitespaces
    while(idx < tn.size() && std::isspace(tn[idx]))
        idx++;

    if(std::isalpha(tn.front()) || tn.front() == '_') {
        while(idx < tn.size() && (std::isalnum(tn[idx]) || tn[idx] == '_'))
            idx++;
        name = tn.substr(0, idx);
    }

    if(name.empty()) {
        ct_exceptf("typing::parse('%.*s'): type is empty",
                   static_cast<int>(tn.size()), tn.data());
    }

    if(idx < tn.size() && tn[idx] == '[') {
        usize startidx = idx + 1;
        while(idx < tn.size() && tn[idx] != ']')
            idx++;

        if(idx >= tn.size()) {
            ct_exceptf("typing::parse('%.*s'): index out of bounds",
                       static_cast<int>(tn.size()), tn.data());
        }

        strn = tn.substr(startidx, idx - startidx);
        idx++; // Skip ']'
    }

    if(!strn.empty()) {
        auto nval = utils::to_integer(strn);

        if(!nval || !nval.value()) {
            ct_exceptf("typing::parse('%.*s'): index valid size",
                       static_cast<int>(tn.size()), tn.data());
        }

        n = *nval;
    }

    // Ignore trailing whitespaces and check for junk
    while(idx < tn.size()) {
        if(std::isspace(idx++)) continue;
        ct_exceptf("typing::parse('%.*s'): index type",
                   static_cast<int>(tn.size()), tn.data());
    }
}

} // namespace

BaseTypes::BaseTypes() {
    // clang-format off
    this->m_registered = {
        { ids::BOOL,  {.name = names::BOOL,  .size = sizeof(bool),     .flags = TD_NONE }},
        { ids::CHAR,  {.name = names::CHAR,  .size = sizeof(char),     .flags = TD_NONE }},
        { ids::WCHAR, {.name = names::WCHAR, .size = sizeof(char16_t), .flags = TD_NONE }},
        { ids::U8,    {.name = names::U8,    .size = sizeof(u8),       .flags = TD_INT }},       
        { ids::U16,   {.name = names::U16,   .size = sizeof(u16),      .flags = TD_INT }},
        { ids::U32,   {.name = names::U32,   .size = sizeof(u32),      .flags = TD_INT }},
        { ids::U64,   {.name = names::U64,   .size = sizeof(u64),      .flags = TD_INT }},
        { ids::I8,    {.name = names::I8,    .size = sizeof(i8),       .flags = TD_INT | TD_SIGNED }},       
        { ids::I16,   {.name = names::I16,   .size = sizeof(i16),      .flags = TD_INT | TD_SIGNED }},
        { ids::I32,   {.name = names::I32,   .size = sizeof(i32),      .flags = TD_INT | TD_SIGNED }},
        { ids::I64,   {.name = names::I64,   .size = sizeof(i64),      .flags = TD_INT | TD_SIGNED }},
        { ids::U16BE, {.name = names::U16,   .size = sizeof(u16),      .flags = TD_INT | TD_BIG }},
        { ids::U32BE, {.name = names::U32,   .size = sizeof(u32),      .flags = TD_INT | TD_BIG }},
        { ids::U64BE, {.name = names::U64,   .size = sizeof(u64),      .flags = TD_INT | TD_BIG }},
        { ids::I16BE, {.name = names::I16,   .size = sizeof(i16),      .flags = TD_INT | TD_SIGNED | TD_BIG }},
        { ids::I32BE, {.name = names::I32,   .size = sizeof(i32),      .flags = TD_INT | TD_SIGNED | TD_BIG }},
        { ids::I64BE, {.name = names::I64,   .size = sizeof(i64),      .flags = TD_INT | TD_SIGNED | TD_BIG }},
        { ids::STR,   {.name = names::STR,   .size = sizeof(char),     .flags = TD_VAR }},
        { ids::WSTR,  {.name = names::WSTR,  .size = sizeof(char16_t), .flags = TD_VAR }},
    };
    // clang-format on
}

tl::optional<RDType> BaseTypes::int_from_bytes(usize n, bool sign) const {
    for(const auto& [id, td] : this->m_registered) {
        if(!td.is_int() || td.is_big()) continue;

        if(td.size == n && sign == td.is_signed())
            return RDType{.id = id, .n = 0};
    }

    return tl::nullopt;
}

usize BaseTypes::size_of(FullTypeName tname, ParsedType* res) const {
    ParsedType pt = this->parse(tname);
    if(res) *res = pt;
    return pt.tdef->size * std::max<usize>(pt.n, 1);
}

usize BaseTypes::size_of(RDType t) const {
    const TypeDef* td = this->get_typedef(t);
    return td->size * std::max<usize>(t.n, 1);
}

const TypeDef* BaseTypes::get_typedef(RDType t,
                                      std::string_view namehint) const {
    auto it = this->m_registered.find(t.id);

    if(it == this->m_registered.end()) {
        if(!namehint.empty()) {
            ct_exceptf("Type '%.*s' not found",
                       static_cast<int>(namehint.size()), namehint.data());
        }
        else
            ct_exceptf("Type id '%x' not found", t.id);
    }

    return &it->second;
}

ParsedType BaseTypes::parse(FullTypeName tn) const {
    std::string_view name;
    usize n{};
    typing::parse_string(tn, name, n);

    const TypeDef* td = this->get_typedef(name);

    if(td->is_var() && n) {
        ct_exceptf("Cannot create an array of variable sized type '%.*s'",
                   static_cast<int>(tn.size()), tn.data());
    }

    return ParsedType{
        .tdef = td,
        .n = n,
    };
}

std::string BaseTypes::to_string(ParsedType pt) const {
    ct_assume(pt.tdef);
    if(pt.n > 0) return fmt::format("{}[{}]", pt.tdef->name, pt.n);
    return pt.tdef->name;
}

std::string BaseTypes::to_string(RDType t) const {
    const TypeDef* td = this->get_typedef(t);
    if(t.n > 0) return fmt::format("{}[{}]", td->name, t.n);
    return td->name;
}

[[nodiscard]] RDType BaseTypes::from_string(FullTypeName tn) const {
    ParsedType pt = this->parse(tn);
    return pt.to_type();
}

TypeName BaseTypes::type_name(RDType t) const {
    auto it = this->m_registered.find(t.id);
    if(it == this->m_registered.end()) ct_exceptf("Type id %x not found", t.id);
    return it->second.name;
}

} // namespace redasm::typing
