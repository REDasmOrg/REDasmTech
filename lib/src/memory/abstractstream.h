#pragma once

#include "../typing/typing.h"
#include "../utils/utils.h"
#include "abstractbuffer.h"
#include <string>
#include <string_view>
#include <tl/optional.hpp>

namespace redasm {

class AbstractStream {
public:
    AbstractStream() = default;
    virtual ~AbstractStream() = default;
    usize seek(usize o);

    [[nodiscard]] tl::optional<typing::Value>
    peek_type(std::string_view tname) const;

    [[nodiscard]] tl::optional<std::string> peek_string(usize n) const;
    [[nodiscard]] tl::optional<std::string> peek_string() const;
    tl::optional<typing::Value> read_type(std::string_view tname);
    tl::optional<std::string> read_string(usize n);
    tl::optional<std::string> read_string();

    [[nodiscard]] tl::optional<bool> peek_bool() const;
    [[nodiscard]] tl::optional<char> peek_char() const;
    [[nodiscard]] tl::optional<u8> peek_u8() const;
    [[nodiscard]] tl::optional<u16> peek_u16(bool big = false) const;
    [[nodiscard]] tl::optional<u32> peek_u32(bool big = false) const;
    [[nodiscard]] tl::optional<u64> peek_u64(bool big = false) const;
    [[nodiscard]] tl::optional<i8> peek_i8() const;
    [[nodiscard]] tl::optional<i16> peek_i16(bool big = false) const;
    [[nodiscard]] tl::optional<i32> peek_i32(bool big = false) const;
    [[nodiscard]] tl::optional<i64> peek_i64(bool big = false) const;

    tl::optional<bool> read_bool();
    tl::optional<char> read_char();
    tl::optional<u8> read_u8();
    tl::optional<u16> read_u16(bool big = false);
    tl::optional<u32> read_u32(bool big = false);
    tl::optional<u64> read_u64(bool big = false);
    tl::optional<i8> read_i8();
    tl::optional<i16> read_i16(bool big = false);
    tl::optional<i32> read_i32(bool big = false);
    tl::optional<i64> read_i64(bool big = false);

    [[nodiscard]] virtual tl::optional<u8> at(usize idx) const = 0;
    [[nodiscard]] virtual usize size() const = 0;

    [[nodiscard]] inline bool at_end() const {
        return this->position >= this->size();
    }

    inline usize move(isize o) { return this->seek(this->position + o); }
    inline void rewind() { this->position = 0; }

    template<typename T>
    tl::optional<T> peek(bool big = false) const {
        if constexpr(std::is_same_v<T, bool>)
            return this->peek_bool();
        if constexpr(std::is_same_v<T, char>)
            return this->peek_char();
        else if constexpr(std::is_same_v<T, u8>)
            return this->peek_u8();
        else if constexpr(std::is_same_v<T, u16>)
            return this->peek_u16(big);
        else if constexpr(std::is_same_v<T, u32>)
            return this->peek_u32(big);
        else if constexpr(std::is_same_v<T, u64>)
            return this->peek_u64(big);
        else if constexpr(std::is_same_v<T, i8>)
            return this->peek_i8();
        else if constexpr(std::is_same_v<T, i16>)
            return this->peek_i16(big);
        else if constexpr(std::is_same_v<T, i32>)
            return this->peek_i32(big);
        else if constexpr(std::is_same_v<T, i64>)
            return this->peek_i64(big);
        else
            static_assert(utils::AlwaysFalseV<T>);
    }

    template<typename T>
    tl::optional<T> read(bool big = false) {
        auto v = this->peek<T>(big);
        if(v)
            this->position += sizeof(T);
        return v;
    }

protected:
    [[nodiscard]] virtual const AbstractBuffer& buffer() const = 0;
    [[nodiscard]] virtual usize index_base() const;

public:
    usize position{};
};

} // namespace redasm
