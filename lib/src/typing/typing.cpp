#include "typing.h"
#include "../error.h"
#include "../utils/utils.h"
#include <spdlog/spdlog.h>

namespace redasm::typing {

ParseResult parse(std::string_view tname) {
    if(tname.empty())
        except("typing::parse('{}'): type is empty", tname);

    std::string_view type, n;
    usize idx = 0;

    if(std::isalpha(tname.front()) || tname.front() == '_') {
        while(idx < tname.size() &&
              (std::isalnum(tname[idx]) || tname[idx] == '_'))
            idx++;
        type = tname.substr(0, idx);
    }

    if(type.empty())
        except("typing::parse('{}'): type is empty", tname);

    if(idx < tname.size() && tname[idx] == '[') {
        usize startidx = idx + 1;
        while(idx < tname.size() && tname[idx] != ']')
            idx++;

        if(idx >= tname.size())
            except("typing::parse('{}'): index out of bounds", tname);

        n = tname.substr(startidx, idx - startidx);
    }

    if(!n.empty()) {
        auto nval = utils::to_integer(n);
        if(!nval || !nval.value())
            except("typing::parse('{}'): invalid size", tname);
        return {type, *nval};
    }

    return {type, 0};
}

usize Types::size_of(std::string_view tname, const typing::Type** res) const {
    auto [type, n] = typing::parse(tname);
    const Type* t = this->get_type(type);

    if(!t)
        except("Cannot get size of type '{}'", type);

    if(res)
        *res = t;

    return n ? t->size * n : t->size;
}

const Type* Types::get_type(std::string_view type) const {
    auto it = this->registered.find(type);
    if(it == this->registered.end())
        except("Type '{}' not found", type);
    return it->second.get();
}

const Type* Types::get_parsed_type(std::string_view tname) const {
    auto [type, _] = typing::parse(tname);
    return this->get_type(type);
}

void Types::declare(const std::string& name, const Type& type) {
    if(this->registered.count(name))
        except("Type '{}' has already been declared");

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

Type Types::create_struct(const std::vector<StructFields>& arg) const {
    Type type{types::STRUCT};

    for(const auto& [tname, name] : arg) {
        const Type* t = nullptr;
        type.size += this->size_of(tname, &t);

        if(t->is_var()) {
            except("Type '{}' size is variable and is not supported in structs",
                   tname);
        }

        type.dict.emplace_back(tname, name);
    }

    return type;
}

std::string_view Types::type_name(typing::types::Tag tag) const {
    for(const auto& [tname, t] : this->registered) {
        if(t->type() == tag)
            return tname;
    }

    unreachable;
}

} // namespace redasm::typing
