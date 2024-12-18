#include "typing.h"
#include "../error.h"
#include "../utils/utils.h"
#include <fmt/core.h>
#include <spdlog/spdlog.h>

namespace redasm::typing {

namespace {

void parse(FullTypeName tn, std::string_view& name, usize& n) {
    if(tn.empty())
        except("typing::parse('{}'): type is empty", tn);

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

    if(name.empty())
        except("typing::parse('{}'): type is empty", tn);

    if(idx < tn.size() && tn[idx] == '[') {
        usize startidx = idx + 1;
        while(idx < tn.size() && tn[idx] != ']')
            idx++;

        if(idx >= tn.size())
            except("typing::parse('{}'): index out of bounds", tn);

        strn = tn.substr(startidx, idx - startidx);
        idx++; // Skip ']'
    }

    if(!strn.empty()) {
        auto nval = utils::to_integer(strn);
        if(!nval || !nval.value())
            except("typing::parse('{}'): invalid size", tn);

        n = *nval;
    }

    // Ignore trailing whitespaces and check for junk
    while(idx < tn.size()) {
        if(std::isspace(idx++))
            continue;
        except("typing::parse('{}'): invalid type", tn);
    }
}

} // namespace

usize Types::size_of(FullTypeName tname, ParsedType* res) const {
    ParsedType pt = this->parse(tname);

    if(res)
        *res = pt;

    return pt.type->size * std::max<usize>(pt.n, 1);
}

usize Types::size_of(RDType t) const {
    const TypeDef* td = this->get_typedef(t);
    return td->size * std::max<usize>(t.n, 1);
}

const TypeDef* Types::get_typedef(RDType t, std::string_view namehint) const {
    auto it = this->registered.find(t.id);

    if(it == this->registered.end()) {
        if(!namehint.empty())
            except("Type '{}' not found", namehint);
        else
            except("Type if '{:x}' not found", t.id);
    }

    return &it->second;
}

ParsedType Types::parse(FullTypeName tname) const {
    std::string_view name;
    usize n{};
    typing::parse(tname, name, n);

    const TypeDef* td = this->get_typedef(name);

    if(td->isvar && n)
        except("Cannot create an array of variable sized type '{}'", tname);

    return ParsedType{
        .type = td,
        .n = n,
    };
}

Types::Types() {
    // clang-format off
    this->registered[ids::BOOL]  = {.name = names::BOOL,  .size = sizeof(bool)};
    this->registered[ids::CHAR]  = {.name = names::CHAR,  .size = sizeof(char)};
    this->registered[ids::WCHAR] = {.name = names::WCHAR, .size = sizeof(char16_t)};
    this->registered[ids::U8]    = {.name = names::U8,    .size = sizeof(u8)};
    this->registered[ids::U16]   = {.name = names::U16,   .size = sizeof(u16)};
    this->registered[ids::U32]   = {.name = names::U32,   .size = sizeof(u32)};
    this->registered[ids::U64]   = {.name = names::U64,   .size = sizeof(u64)};
    this->registered[ids::I8]    = {.name = names::I8,    .size = sizeof(i8)};
    this->registered[ids::I16]   = {.name = names::I16,   .size = sizeof(i16)};
    this->registered[ids::I32]   = {.name = names::I32,   .size = sizeof(i32)};
    this->registered[ids::I64]   = {.name = names::I64,   .size = sizeof(i64)};
    this->registered[ids::U16BE] = {.name = names::U16,   .size = sizeof(u16),      .isbig = true};
    this->registered[ids::U32BE] = {.name = names::U32,   .size = sizeof(u32),      .isbig = true};
    this->registered[ids::U64BE] = {.name = names::U64,   .size = sizeof(u64),      .isbig = true};
    this->registered[ids::I16BE] = {.name = names::I16,   .size = sizeof(i16),      .isbig = true};
    this->registered[ids::I32BE] = {.name = names::I32,   .size = sizeof(i32),      .isbig = true};
    this->registered[ids::I64BE] = {.name = names::I64,   .size = sizeof(i64),      .isbig = true};
    this->registered[ids::STR]   = {.name = names::STR,   .size = sizeof(char),     .isvar = true};
    this->registered[ids::WSTR]  = {.name = names::WSTR,  .size = sizeof(char16_t), .isvar = true};
    // clang-format on
}

std::string Types::to_string(ParsedType pt) const {
    assume(pt.type);

    if(pt.n > 0)
        return fmt::format("{}[{}]", pt.type->name, pt.n);

    return pt.type->name;
}

std::string Types::to_string(RDType t) const {
    const TypeDef* td = this->get_typedef(t);

    if(t.n > 0)
        return fmt::format("{}[{}]", td->name, t.n);

    return td->name;
}

const TypeDef* Types::declare(const std::string& name, const StructBody& arg) {
    if(name.empty())
        except("Struct name is empty");

    if(arg.empty())
        except("Struct cannot be empty");

    TypeDef type;
    type.name = name;

    if(this->registered.contains(type.get_id()))
        except("Struct '{}' already exists", name);

    for(const auto& [tname, name] : arg) {
        ParsedType pt;
        type.size += this->size_of(tname, &pt);

        if(pt.type->isvar) {
            except("Type '{}' size is variable and is not supported in structs",
                   tname);
        }

        type.dict.emplace_back(RDType{.id = pt.type->get_id(), .n = pt.n},
                               name);
    }

    auto it = this->registered.try_emplace(type.get_id(), type);
    assume(it.second);
    return &it.first->second;
}

TypeName Types::type_name(RDType t) const {
    auto it = this->registered.find(t.id);

    if(it == this->registered.end())
        except("Type id {:x} not found", t.id);

    return it->second.name;
}

} // namespace redasm::typing
