#pragma once

#include "../typing/typing.h"
#include "../utils/object.h"
#include "byteorder.h"
#include <redasm/types.h>
#include <string_view>
#include <tl/optional.hpp>
#include <utility>
#include <vector>

namespace redasm {

class AbstractBuffer: public Object {
public:
    explicit AbstractBuffer(std::string src): source{std::move(src)} {}

    [[nodiscard]] virtual tl::optional<u8> get_byte(usize idx) const = 0;
    [[nodiscard]] virtual usize size() const = 0;

    [[nodiscard]] tl::optional<std::string> get_str(usize idx) const;
    [[nodiscard]] tl::optional<std::string> get_str(usize idx, usize n) const;

    [[nodiscard]] tl::optional<std::string> get_wstr(usize idx) const;
    [[nodiscard]] tl::optional<std::string> get_wstr(usize idx, usize n) const;

    [[nodiscard]] tl::optional<u8> get_u8(usize idx) const {
        return this->get_byte(idx);
    }

    [[nodiscard]] tl::optional<bool> get_bool(usize idx) const {
        auto b = this->get_byte(idx);
        if(b)
            return !!(*b);
        return tl::nullopt;
    }

    [[nodiscard]] tl::optional<char> get_char(usize idx) const {
        auto b = this->get_byte(idx);
        if(b)
            return static_cast<char>(*b);
        return tl::nullopt;
    }

    [[nodiscard]] tl::optional<char> get_wchar(usize idx) const {
        auto b = this->get_u16(idx, false);
        if(b)
            return static_cast<char>(*b & 0xFF);
        return tl::nullopt;
    }

    [[nodiscard]] tl::optional<u16> get_u16(usize idx, bool big) const {
        return this->get_number<u16>(idx, big);
    }

    [[nodiscard]] tl::optional<u32> get_u32(usize idx, bool big) const {
        return this->get_number<u32>(idx, big);
    }

    [[nodiscard]] tl::optional<u64> get_u64(usize idx, bool big) const {
        return this->get_number<u64>(idx, big);
    }

    [[nodiscard]] tl::optional<i8> get_i8(usize idx) const {
        if(auto b = this->get_u8(idx); b)
            return static_cast<i8>(*b);
        return tl::nullopt;
    }

    [[nodiscard]] tl::optional<i16> get_i16(usize idx, bool big) const {
        if(auto b = this->get_u16(idx, big); b)
            return static_cast<i16>(*b);
        return tl::nullopt;
    }

    [[nodiscard]] tl::optional<i32> get_i32(usize idx, bool big) const {
        if(auto b = this->get_u32(idx, big); b)
            return static_cast<i32>(*b);
        return tl::nullopt;
    }

    [[nodiscard]] tl::optional<i64> get_i64(usize idx, bool big) const {
        if(auto b = this->get_u64(idx, big); b)
            return static_cast<i64>(*b);
        return tl::nullopt;
    }

    [[nodiscard]] tl::optional<typing::Value>
    get_type(usize idx, typing::FullTypeName tn) const;

    [[nodiscard]]
    tl::optional<typing::Value> get_type(usize idx, RDType t) const;

    [[nodiscard]] tl::optional<typing::Value>
    get_type(usize idx, typing::FullTypeName tn, usize& lastidx) const;

    [[nodiscard]] tl::optional<typing::Value> get_type(usize idx, RDType t,
                                                       usize& lastidx) const;

private:
    template<typename U>
    [[nodiscard]] tl::optional<U> get_number(usize idx,
                                             bool big = false) const {
        static constexpr usize N = sizeof(U);

        if(idx + N >= this->size())
            return tl::nullopt;

        U num = 0;

        for(usize i = 0; i < N; i++) {
            if(auto b = this->get_byte(idx + i); b)
                num |= (static_cast<U>(*b) << (i * CHAR_BIT));
            else
                return tl::nullopt;
        }

        if constexpr(sizeof(U) == sizeof(u8))
            return num;
        else {
            if(big)
                return byteorder::to_bigendian(num);

            return byteorder::to_littleendian(num);
        }
    }

    tl::optional<std::string> get_str_impl(usize& idx,
                                           std::string_view tname) const;

    tl::optional<std::string> get_str_impl(usize& idx, usize n,
                                           typing::TypeName tname) const;

    tl::optional<typing::Value> get_type_impl(usize& idx, RDType t) const;

    tl::optional<typing::Value> get_type_impl(usize& idx,
                                              const typing::TypeDef* t) const;

public:
    std::string source;
};

template<typename T>
class AbstractBufferT: public AbstractBuffer {
public:
    using Type = std::vector<T>;

    auto begin() const { return m_buffer.begin(); }
    auto end() const { return m_buffer.end(); }
    auto begin() { return m_buffer.begin(); }
    auto end() { return m_buffer.end(); }
    void resize(usize sz) { m_buffer.resize(sz); }
    [[nodiscard]] usize size() const final { return m_buffer.size(); }

    explicit AbstractBufferT(std::string src): AbstractBuffer{std::move(src)} {}
    T& at(usize idx) { return m_buffer.at(idx); }
    T at(usize idx) const { return m_buffer.at(idx); }
    T operator[](usize idx) const { return m_buffer.at(idx); }
    T& operator[](usize idx) { return m_buffer.at(idx); }
    const T* data() const { return m_buffer.data(); }

protected:
    Type m_buffer;
};

} // namespace redasm
