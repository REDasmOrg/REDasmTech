#pragma once

#include <fmt/core.h>
#include <map>
#include <memory>
#include <redasm/types.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace redasm::typing {

namespace types {

using Tag = u8;

// clang-format off
enum: Tag {
    NIL      = 0b00000000,

    BOOL, CHAR, WCHAR,
    U8, U16, U32, U64,
    I8, I16, I32, I64,
    STR, WSTR,
    STRUCT,

    BIG       = 0b10000000,
    VAR       = 0b01000000,
    TYPE_MASK = 0b00111111,
};
// clang-format on

} // namespace types

namespace names {

inline constexpr const char* BOOL = "bool";
inline constexpr const char* CHAR = "char";
inline constexpr const char* WCHAR = "wchar";
inline constexpr const char* U8 = "u8";
inline constexpr const char* U16 = "u16";
inline constexpr const char* U32 = "u32";
inline constexpr const char* U64 = "u64";
inline constexpr const char* I8 = "u8";
inline constexpr const char* I16 = "u16";
inline constexpr const char* I32 = "u32";
inline constexpr const char* I64 = "u64";
inline constexpr const char* U16BE = "u16be";
inline constexpr const char* U32BE = "u32be";
inline constexpr const char* U64BE = "u64be";
inline constexpr const char* I16BE = "u16be";
inline constexpr const char* I32BE = "u32be";
inline constexpr const char* I64BE = "u64be";
inline constexpr const char* STR = "str";
inline constexpr const char* WSTR = "wstr";

} // namespace names

struct Type {
    u8 tag{types::NIL};
    usize size{};
    std::string name;

    std::vector<std::pair<std::string, std::string>> dict;

    explicit Type(u8 t): tag{t} {}
    explicit Type(u8 t, usize s): tag{t}, size{s} {}
    inline u8 type() const { return tag & types::TYPE_MASK; }
    inline bool is_var() const { return tag & types::VAR; }

    inline bool is_char() const {
        return this->type() == types::CHAR || this->type() == types::WCHAR;
    }

    inline bool is_str() const {
        return this->type() == types::STR || this->type() == types::WSTR;
    }

    inline bool is_wide() const {
        return this->type() == types::WCHAR || this->type() == types::WSTR;
    }

    inline bool is_big() const {
        return (this->type() >= types::U8 && this->type() <= types::I64) &&
               (tag & types::BIG);
    }
};

struct Value {
    std::string type;
    usize count{0};

    std::vector<Value> list;
    std::unordered_map<std::string, Value> dict;
    std::string str;

    union {
        bool b_v;
        char ch_v;
        u8 u8_v;
        u16 u16_v;
        u32 u32_v;
        u64 u64_v;
        i8 i8_v;
        i16 i16_v;
        i32 i32_v;
        i64 i64_v;
    };

    Value() = default;
    explicit Value(std::string t, usize c): type{std::move(t)}, count{c} {}
    explicit Value(std::string t): type{std::move(t)} {}
    inline bool is_list() const { return count > 0; }
};
// clang-format on

using ParseResult = std::pair<std::string_view, size_t>;
ParseResult parse(std::string_view tname);

struct Types {
    using StructFields = std::pair<std::string, std::string>;

    Types();
    void declare(const std::string& name, const Type& type);
    [[nodiscard]] const Type* get_type(std::string_view type) const;
    [[nodiscard]] const Type* get_parsed_type(std::string_view tname) const;
    [[nodiscard]] std::string_view type_name(types::Tag tag) const;

    [[nodiscard]] Type
    create_struct(const std::vector<StructFields>& arg) const;

    [[nodiscard]] usize size_of(std::string_view tname,
                                const typing::Type** res = nullptr) const;

    [[nodiscard]] inline std::string as_array(std::string_view tname,
                                              usize n) const {
        return fmt::format("{}[{}]", tname, n);
    }

    [[nodiscard]] inline std::string as_array(types::Tag t, usize n) const {
        return this->as_array(this->type_name(t), n);
    }

    std::map<std::string, std::unique_ptr<Type>, std::less<>> registered;
};

using StructBody = std::vector<typing::Types::StructFields>;

} // namespace redasm::typing
