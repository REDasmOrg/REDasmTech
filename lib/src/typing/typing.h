#pragma once

#include "../utils/hash.h"
#include "../utils/object.h"
#include <fmt/core.h>
#include <forward_list>
#include <map>
#include <redasm/typing.h>
#include <string>
#include <string_view>
#include <tl/optional.hpp>
#include <unordered_map>
#include <vector>

namespace redasm::typing {

namespace names {

inline constexpr const char* BOOL = "bool";
inline constexpr const char* CHAR = "char";
inline constexpr const char* WCHAR = "wchar";
inline constexpr const char* U8 = "u8";
inline constexpr const char* U16 = "u16";
inline constexpr const char* U32 = "u32";
inline constexpr const char* U64 = "u64";
inline constexpr const char* I8 = "i8";
inline constexpr const char* I16 = "i16";
inline constexpr const char* I32 = "i32";
inline constexpr const char* I64 = "i64";
inline constexpr const char* U16BE = "u16be";
inline constexpr const char* U32BE = "u32be";
inline constexpr const char* U64BE = "u64be";
inline constexpr const char* I16BE = "i16be";
inline constexpr const char* I32BE = "i32be";
inline constexpr const char* I64BE = "i64be";
inline constexpr const char* STR = "str";
inline constexpr const char* WSTR = "wstr";

} // namespace names

namespace ids {

inline constexpr u32 BOOL = hash::static_murmur3(typing::names::BOOL);
inline constexpr u32 CHAR = hash::static_murmur3(typing::names::CHAR);
inline constexpr u32 WCHAR = hash::static_murmur3(typing::names::WCHAR);
inline constexpr u32 U8 = hash::static_murmur3(typing::names::U8);
inline constexpr u32 U16 = hash::static_murmur3(typing::names::U16);
inline constexpr u32 U32 = hash::static_murmur3(typing::names::U32);
inline constexpr u32 U64 = hash::static_murmur3(typing::names::U64);
inline constexpr u32 I8 = hash::static_murmur3(typing::names::I8);
inline constexpr u32 I16 = hash::static_murmur3(typing::names::I16);
inline constexpr u32 I32 = hash::static_murmur3(typing::names::I32);
inline constexpr u32 I64 = hash::static_murmur3(typing::names::I64);
inline constexpr u32 U16BE = hash::static_murmur3(typing::names::U16BE);
inline constexpr u32 U32BE = hash::static_murmur3(typing::names::U32BE);
inline constexpr u32 U64BE = hash::static_murmur3(typing::names::U64BE);
inline constexpr u32 I16BE = hash::static_murmur3(typing::names::I16BE);
inline constexpr u32 I32BE = hash::static_murmur3(typing::names::I32BE);
inline constexpr u32 I64BE = hash::static_murmur3(typing::names::I64BE);
inline constexpr u32 STR = hash::static_murmur3(typing::names::STR);
inline constexpr u32 WSTR = hash::static_murmur3(typing::names::WSTR);

} // namespace ids

using FullTypeName = std::string_view;
using TypeName = std::string_view;

enum TypedefFlags {
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

class Value;
using ValueList = std::vector<Value>;
using ValueDict = std::map<std::string, Value, std::less<>>;
using StructField = std::pair<std::string, std::string>;
using StructBody = std::vector<typing::StructField>;

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
