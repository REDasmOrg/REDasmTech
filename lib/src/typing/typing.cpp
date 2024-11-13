#include "typing.h"
#include "../error.h"
#include "../utils/utils.h"
#include <fmt/core.h>
#include <spdlog/spdlog.h>

namespace redasm::typing {

namespace {

void parse(std::string_view tname, std::string_view& name, usize& n,
           usize& modifier) {
    if(tname.empty())
        except("typing::parse('{}'): type is empty", tname);

    modifier = TYPEMODIFIER_NORMAL;

    usize idx = 0;
    std::string_view strn;

    // Ignore leading whitespaces
    while(idx < tname.size() && std::isspace(tname[idx]))
        idx++;

    if(std::isalpha(tname.front()) || tname.front() == '_') {
        while(idx < tname.size() &&
              (std::isalnum(tname[idx]) || tname[idx] == '_'))
            idx++;
        name = tname.substr(0, idx);
    }

    if(name.empty())
        except("typing::parse('{}'): type is empty", tname);

    if(idx < tname.size() && tname[idx] == '*') {
        modifier = TYPEMODIFIER_POINTER;
        idx++;
    }
    else if(idx < tname.size() && tname[idx] == '^') {
        modifier = TYPEMODIFIER_RELPOINTER;
        idx++;
    }

    if(idx < tname.size() && tname[idx] == '[') {
        usize startidx = idx + 1;
        while(idx < tname.size() && tname[idx] != ']')
            idx++;

        if(idx >= tname.size())
            except("typing::parse('{}'): index out of bounds", tname);

        strn = tname.substr(startidx, idx - startidx);
        idx++; // Skip ']'
    }

    if(!strn.empty()) {
        auto nval = utils::to_integer(strn);
        if(!nval || !nval.value())
            except("typing::parse('{}'): invalid size", tname);

        n = *nval;
    }

    // Ignore trailing whitespaces and check for junk
    while(idx < tname.size()) {
        if(std::isspace(idx++))
            continue;
        except("typing::parse('{}'): invalid type", tname);
    }
}

} // namespace

usize Types::size_of(std::string_view tname, ParsedType* res) const {
    auto pt = this->parse(tname);

    if(!pt)
        except("Cannot get size of type '{}'", pt->type->name);

    if(res)
        *res = *pt;

    return pt->n ? pt->type->size * pt->n : pt->type->size;
}

const Type* Types::get_type(std::string_view type) const {
    auto it = this->registered.find(type);
    if(it == this->registered.end())
        except("Type '{}' not found", type);
    return it->second.get();
}

tl::optional<ParsedType> Types::parse(std::string_view tname) const {
    std::string_view name;
    usize n{}, modifier{};
    typing::parse(tname, name, n, modifier);

    const Type* t = this->get_type(name);

    if(t) {
        return ParsedType{
            .type = t,
            .n = n,
            .modifier = modifier,
        };
    }

    return tl::nullopt;
}

void Types::declare(const std::string& name, const Type& type) {
    if(this->registered.contains(name))
        except("Type '{}' has already been declared", name);

    auto newtype = std::make_unique<Type>(type);
    newtype->name = name;
    this->registered[name] = std::move(newtype);
}

Types::Types() {
    // clang-format off
    this->registered[names::BOOL]  = std::make_unique<Type>(types::BOOL,  sizeof(bool));
    this->registered[names::CHAR]  = std::make_unique<Type>(types::CHAR,  sizeof(char));
    this->registered[names::WCHAR] = std::make_unique<Type>(types::WCHAR, sizeof(char16_t));
    this->registered[names::U8]    = std::make_unique<Type>(types::U8,    sizeof(u8));
    this->registered[names::U16]   = std::make_unique<Type>(types::U16,   sizeof(u16));
    this->registered[names::U32]   = std::make_unique<Type>(types::U32,   sizeof(u32));
    this->registered[names::U64]   = std::make_unique<Type>(types::U64,   sizeof(u64));
    this->registered[names::I8]    = std::make_unique<Type>(types::I8,    sizeof(i8));
    this->registered[names::I16]   = std::make_unique<Type>(types::I16,   sizeof(i16));
    this->registered[names::I32]   = std::make_unique<Type>(types::I32,   sizeof(i32));
    this->registered[names::I64]   = std::make_unique<Type>(types::I64,   sizeof(i64));
    this->registered[names::U16BE] = std::make_unique<Type>(types::U16  | types::BIG, sizeof(u16));
    this->registered[names::U32BE] = std::make_unique<Type>(types::U32  | types::BIG, sizeof(u32));
    this->registered[names::U64BE] = std::make_unique<Type>(types::U64  | types::BIG, sizeof(u64));
    this->registered[names::I16BE] = std::make_unique<Type>(types::I16  | types::BIG, sizeof(i16));
    this->registered[names::I32BE] = std::make_unique<Type>(types::I32  | types::BIG, sizeof(i32));
    this->registered[names::I64BE] = std::make_unique<Type>(types::I64  | types::BIG, sizeof(i64));
    this->registered[names::STR]   = std::make_unique<Type>(types::STR  | types::VAR, sizeof(char));
    this->registered[names::WSTR]  = std::make_unique<Type>(types::WSTR | types::VAR, sizeof(char16_t));
    // clang-format on

    for(auto& [tname, t] : this->registered)
        t->name = tname;
}

std::string Types::to_string(const ParsedType& pt) {
    assume(pt.type);

    std::string_view mod;

    switch(pt.modifier) {
        case TYPEMODIFIER_RELPOINTER: mod = "^"; break;
        case TYPEMODIFIER_POINTER: mod = "*"; break;
        case TYPEMODIFIER_NORMAL: break;
        default: unreachable;
    }

    if(pt.n > 0)
        return fmt::format("{}{}[{}]", pt.type->name, mod, pt.n);

    return fmt::format("{}{}", pt.type->name, mod);
}

Type Types::create_struct(const std::vector<StructFields>& arg) const {
    Type type{types::STRUCT};

    for(const auto& [tname, name] : arg) {
        ParsedType pt;
        type.size += this->size_of(tname, &pt);

        if(pt.type->is_var()) {
            except("Type '{}' size is variable and is not supported in structs",
                   tname);
        }

        type.dict.emplace_back(tname, name);
    }

    return type;
}

std::string_view Types::type_name(typing::types::Tag tag) const {
    for(const auto& [tname, t] : this->registered) {
        if(t->id() == tag)
            return tname;
    }

    unreachable;
}

} // namespace redasm::typing
