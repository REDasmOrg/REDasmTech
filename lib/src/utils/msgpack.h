#pragma once

#define MSGPACK_NAMESPACE redasm

// https://github.com/msgpack/msgpack/blob/master/spec.md

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <limits>
#include <map>
#include <string>
#include <string_view>
#include <sys/param.h>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

#if defined(__EXCEPTIONS) || defined(_CPPUNWIND)
#include <stdexcept>
#endif

#if defined(__BYTE_ORDER)
#if defined(__BIG_ENDIAN) && (__BYTE_ORDER == __BIG_ENDIAN)
#define MSGPACK_BIG_ENDIAN
#elif defined(__LITTLE_ENDIAN) && (__BYTE_ORDER == __LITTLE_ENDIAN)
#define MSGPACK_LITTLE_ENDIAN
#else
static_assert(false, "MsgPack: Unsupported endianness")
#endif
#else
static_assert(false, "MsgPack: Cannot detect byte order")
#endif

#if defined(MSGPACK_NAMESPACE)
namespace MSGPACK_NAMESPACE {
#endif

namespace msgpack {

template<typename Container>
struct BasicVisitor {
    using ContainerType = Container;
    using IntegerType = std::variant<int8_t, uint8_t, int16_t, uint16_t,
                                     int32_t, uint32_t, int64_t, uint64_t>;

    bool start_map(size_t /* size */) { return true; }
    bool end_map() { return true; }
    bool start_map_key(size_t /* index */) { return true; }
    bool end_map_key(size_t /* index */) { return true; }
    bool start_map_value(size_t /* index */) { return true; }
    bool end_map_value(size_t /* index */) { return true; }
    bool start_array(size_t /* size */) { return true; }
    bool end_array() { return true; }
    bool start_array_item(size_t /* index */) { return true; }
    bool end_array_item(size_t /* index */) { return true; }
    bool visit_nil() { return true; }
    bool visit_bool(bool /* arg */) { return true; }
    bool visit_str(std::string_view /* arg */) { return true; }
    bool visit_int(IntegerType /* arg */) { return true; }
    bool visit_bin(const ContainerType& /* arg */) { return true; }

    bool visit_ext(int8_t /*type*/, const ContainerType& /* arg */) {
        return true;
    }
};

namespace impl {

template<typename>
inline constexpr bool always_false_v = false; // NOLINT

template<typename T>
inline constexpr bool is_bool_v = std::is_same_v<T, bool>; // NOLINT

template<typename T>
inline constexpr bool is_array_v = false; // NOLINT

template<typename T, typename Allocator>
inline constexpr bool is_array_v<std::vector<T, Allocator>> = true; // NOLINT

template<typename T, std::size_t N>
inline constexpr bool is_array_v<std::array<T, N>> = true; // NOLINT

template<typename T>
inline constexpr bool is_vector_v = false; // NOLINT

template<typename T, typename Allocator>
inline constexpr bool is_vector_v<std::vector<T, Allocator>> = true; // NOLINT

template<typename T>
inline constexpr bool is_map_v = false; // NOLINT

template<typename K, typename V, typename Compare, typename Allocator>
inline constexpr bool is_map_v<std::map<K, V, Compare, Allocator>> = // NOLINT
    true;

template<typename K, typename V, typename Compare, typename Allocator>
inline constexpr bool
    is_map_v<std::unordered_map<K, V, Compare, Allocator>> = // NOLINT
    true;

template<typename T>
inline constexpr bool is_string_v = false; // NOLINT

template<>
inline constexpr bool is_string_v<std::string> = true; // NOLINT

template<>
inline constexpr bool is_string_v<std::string_view> = true; // NOLINT

template<>
inline constexpr bool is_string_v<const char*> = true; // NOLINT

struct Format {
    using Type = uint8_t;

    enum : Type {
        NIL = 0xc0,
        FIXMAP = 0x80,
        FIXARRAY = 0x90,
        FIXSTR = 0xa0,
        FALSE = 0xc2,
        TRUE = 0xc3,
        BIN8 = 0xc4,
        BIN16 = 0xc5,
        BIN32 = 0xc6,
        EXT8 = 0xc7,
        EXT16 = 0xc8,
        EXT32 = 0xc9,
        UINT8 = 0xcc,
        UINT16 = 0xcd,
        UINT32 = 0xce,
        UINT64 = 0xcf,
        INT8 = 0xd0,
        INT16 = 0xd1,
        INT32 = 0xd2,
        INT64 = 0xd3,
        FIXEXT1 = 0xd4,
        FIXEXT2 = 0xd5,
        FIXEXT4 = 0xd6,
        FIXEXT8 = 0xd7,
        FIXEXT16 = 0xd8,
        STR8 = 0xd9,
        STR16 = 0xda,
        STR32 = 0xdb,
        ARRAY16 = 0xdc,
        ARRAY32 = 0xdd,
        MAP16 = 0xde,
        MAP32 = 0xdf,
    };
};

[[noreturn]] inline void msgpack_except(const char* errmsg) {
#if defined(__EXCEPTIONS) || defined(_CPPUNWIND)
    throw std::runtime_error(errmsg);
#elif !defined(_NDEBUG)
        assert(false)
#else
    std::abort();
#endif
}

template<typename T>
T swap_bigendian(T t) {
#if defined(MSGPACK_LITTLE_ENDIAN)
    union {
        T u;
        uint8_t b[sizeof(T)];
    } source{t}, dest;

    for(size_t i = 0; i < sizeof(T); i++)
        dest.b[i] = source.b[sizeof(T) - i - 1];

    return dest.u;
#else
        return t;
#endif
}

template<typename T, typename MsgPackType>
bool visit_type(MsgPackType& mp, T& t) {
    if(mp.at_end())
        return false;
    mp.unpack(t);
    return true;
}

template<typename MsgPackType, typename VisitorType>
bool visit(MsgPackType& mp, VisitorType&& visitor);

template<typename MsgPackType, typename VisitorType>
bool visit_bin(MsgPackType& mp, VisitorType&& visitor) {
    if(mp.at_end())
        return false;
    auto c = mp.unpack_bin();
    return visitor.visit_bin(c);
}

template<typename MsgPackType, typename VisitorType>
bool visit_ext(MsgPackType& mp, VisitorType&& visitor) {
    if(mp.at_end())
        return false;
    auto [type, ext] = mp.unpack_ext();
    return visitor.visit_ext(type, ext);
}

template<typename MsgPackType, typename VisitorType>
bool visit_array(MsgPackType& mp, VisitorType&& visitor) {
    if(mp.at_end())
        return false;

    size_t s = mp.unpack_array();
    if(!visitor.start_array(s))
        return false;

    for(size_t i = 0; i < s; ++i) {
        if(!visitor.start_array_item(i) ||
           !impl::visit(mp, std::forward<VisitorType>(visitor)) ||
           !visitor.end_array_item(i))
            return false;
    }

    return visitor.end_array();
}

template<typename MsgPackType, typename VisitorType>
bool visit_map(MsgPackType& mp, VisitorType&& visitor) {
    if(mp.at_end())
        return false;

    size_t s = mp.unpack_map();
    if(!visitor.start_map(s))
        return false;

    for(size_t i = 0; i < s; ++i) {
        if(!visitor.start_map_key(i) ||
           !impl::visit(mp, std::forward<VisitorType>(visitor)) ||
           !visitor.end_map_key(i))
            return false;
        if(!visitor.start_map_value(i) ||
           !impl::visit(mp, std::forward<VisitorType>(visitor)) ||
           !visitor.end_map_value(i))
            return false;
    }

    return visitor.end_map();
}

template<typename MsgPackType, typename VisitorType>
bool visit(MsgPackType& mp, VisitorType&& visitor) {
    if(mp.at_end())
        return false;

    auto f = static_cast<uint8_t>(mp.buffer.get()[mp.pos]);

    if((f & 0xF0) == impl::Format::FIXMAP)
        return impl::visit_map(mp, visitor);
    if((f & 0xF0) == impl::Format::FIXARRAY)
        return impl::visit_array(mp, visitor);
    if((f & 0xE0) == impl::Format::FIXSTR) {
        std::string_view v;
        return impl::visit_type(mp, v) && visitor.visit_str(v);
    }

    switch(f) {
        case impl::Format::MAP16:
        case impl::Format::MAP32: return impl::visit_map(mp, visitor);

        case impl::Format::ARRAY16:
        case impl::Format::ARRAY32: return impl::visit_array(mp, visitor);

        case impl::Format::FIXEXT1:
        case impl::Format::FIXEXT2:
        case impl::Format::FIXEXT4:
        case impl::Format::FIXEXT8:
        case impl::Format::EXT8:
        case impl::Format::EXT16:
        case impl::Format::EXT32: return impl::visit_ext(mp, visitor);

        case impl::Format::BIN8:
        case impl::Format::BIN16:
        case impl::Format::BIN32: return impl::visit_bin(mp, visitor);

        case impl::Format::STR8:
        case impl::Format::STR16:
        case impl::Format::STR32: {
            std::string_view v;
            return impl::visit_type(mp, v) && visitor.visit_str(v);
        }

        case impl::Format::UINT8: {
            uint8_t v;
            return impl::visit_type(mp, v) && visitor.visit_int(v);
        }
        case impl::Format::UINT16: {
            uint16_t v;
            return impl::visit_type(mp, v) && visitor.visit_int(v);
        }
        case impl::Format::UINT32: {
            uint32_t v;
            return impl::visit_type(mp, v) && visitor.visit_int(v);
        }
        case impl::Format::UINT64: {
            uint64_t v;
            return impl::visit_type(mp, v) && visitor.visit_int(v);
        }
        case impl::Format::INT8: {
            int8_t v;
            return impl::visit_type(mp, v) && visitor.visit_int(v);
        }
        case impl::Format::INT16: {
            int16_t v;
            return impl::visit_type(mp, v) && visitor.visit_int(v);
        }
        case impl::Format::INT32: {
            int32_t v;
            return impl::visit_type(mp, v) && visitor.visit_int(v);
        }
        case impl::Format::INT64: {
            int64_t v;
            return impl::visit_type(mp, v) && visitor.visit_int(v);
        }
        case impl::Format::TRUE: {
            ++mp.pos;
            return visitor.visit_bool(true);
        }
        case impl::Format::FALSE: {
            ++mp.pos;
            return visitor.visit_bool(false);
        }
        case impl::Format::NIL: {
            ++mp.pos;
            return visitor.visit_nil();
        }

        default: {
            ++mp.pos;
            int sel = static_cast<int>(f);

            if(sel >= 0x00 && sel <= 0x7F) // Positive FIXNUM
                return visitor.visit_int(static_cast<uint8_t>(f));
            if(sel >= 0xe0 && sel <= 0xFF) // Negative FIXNUM
                return visitor.visit_int(static_cast<int8_t>(f));

            impl::msgpack_except("msgpack::visit(): Invalid Format");
            break;
        }
    }

    return false;
}

} // namespace impl

template<typename Container>
struct BasicMsgPack {
    using Type = BasicMsgPack<Container>;
    using ContainerType = Container;
    using ValueType = typename ContainerType::value_type;

    static_assert(sizeof(ValueType) == sizeof(uint8_t));

    explicit BasicMsgPack() = delete;
    explicit BasicMsgPack(ContainerType& c): buffer{c} {}
    explicit BasicMsgPack(const ContainerType& c)
        : buffer{const_cast<ContainerType&>(c)}, m_readonly{true} {}

    [[nodiscard]] inline size_t size() const { return this->buffer().size(); }
    inline void rewind() { this->pos = 0; }

    inline void push(const ContainerType& c) {
        std::copy(c.cbegin(), c.cend(), std::back_inserter(this->buffer.get()));
    }

    [[nodiscard]] inline bool at_end() const {
        return this->pos >= this->buffer.get().size();
    }

    Type& pack_bin(const ValueType* data, size_t size) {
        assert(data);

        if(size <= std::numeric_limits<std::uint8_t>::max()) {
            this->pack_format(impl::Format::BIN8);
            this->pack_int(static_cast<uint8_t>(size));
        }
        else if(size <= std::numeric_limits<std::uint16_t>::max()) {
            this->pack_format(impl::Format::BIN16);
            this->pack_int(static_cast<uint16_t>(size));
        }
        else {
            this->pack_format(impl::Format::BIN32);
            this->pack_int(static_cast<uint32_t>(size));
        }

        this->pack_raw(data, size);
        return *this;
    }

    Type& pack_ext(int8_t t, const ValueType* data, size_t size) {
        assert(data);
        if(t < 0)
            impl::msgpack_except("MsgPack::pack_ext(): type < 0 is reserved");

        auto packfixext = [&](uint8_t fmt) {
            this->pack_format(fmt);
            this->pack_format(static_cast<uint8_t>(t));
        };

        switch(size) {
            case 1: packfixext(impl::Format::FIXEXT1); break;
            case 2: packfixext(impl::Format::FIXEXT2); break;
            case 4: packfixext(impl::Format::FIXEXT4); break;
            case 8: packfixext(impl::Format::FIXEXT8); break;
            case 16: packfixext(impl::Format::FIXEXT16); break;

            default: {
                if(size <= std::numeric_limits<std::uint8_t>::max()) {
                    this->pack_format(impl::Format::EXT8);
                    this->pack_int(static_cast<uint8_t>(size));
                    this->pack_format(static_cast<uint8_t>(t));
                }
                else if(size <= std::numeric_limits<std::uint16_t>::max()) {
                    this->pack_format(impl::Format::EXT16);
                    this->pack_int(static_cast<uint16_t>(size));
                    this->pack_format(static_cast<uint8_t>(t));
                }
                else {
                    this->pack_format(impl::Format::EXT32);
                    this->pack_int(static_cast<uint32_t>(size));
                    this->pack_format(static_cast<uint8_t>(t));
                }
                break;
            }
        }

        this->pack_raw(data, size);
        return *this;
    }

    template<typename ExtCallback>
    Type& pack_ext(int8_t t, ExtCallback cb) {
        ContainerType ext;
        cb(Type{ext});
        return this->pack_ext(t, ext.data(), ext.size());
    }

    template<typename T>
    Type& pack(T&& t) {
        using U = std::decay_t<T>;

        if constexpr(impl::is_array_v<U>) {
            this->pack_array(t.size());
            for(const auto& v : t)
                this->pack(v);
        }
        else if constexpr(impl::is_map_v<U>) {
            this->pack_map(t.size());
            for(const auto& [key, value] : t) {
                this->pack(key);
                this->pack(value);
            }
        }
        else if constexpr(impl::is_string_v<U>)
            this->pack_string(t);
        else if constexpr(impl::is_bool_v<U>)
            this->pack_bool(t);
        else if constexpr(std::is_enum_v<U>)
            this->pack_int(static_cast<std::underlying_type_t<U>>(t));
        else if constexpr(std::is_integral_v<U>)
            this->pack_int(t);
        else if constexpr(std::is_null_pointer_v<U>)
            this->pack_format(impl::Format::NIL);
        else
            static_assert(impl::always_false_v<U>,
                          "MsgPack::pack(): Unsupported type");
        return *this;
    }

    template<typename T>
    inline T unpack() {
        T t;
        this->unpack(t);
        return t;
    }

    inline ContainerType unpack_bin() {
        ContainerType c;
        this->unpack_bin(c);
        return c;
    }

    void unpack_bin(ContainerType& c) {
        uint8_t f = this->unpack_format();

        switch(f) {
            case impl::Format::BIN8: {
                uint8_t n;
                this->unpack_int(n);
                c.resize(n);
                break;
            }

            case impl::Format::BIN16: {
                uint16_t n;
                this->unpack_int(n);
                c.resize(n);
                break;
            }

            case impl::Format::BIN32: {
                uint32_t n;
                this->unpack_int(n);
                c.resize(n);
                break;
            }

            default:
                impl::msgpack_except(
                    "MsgPack::unpack_int(): Invalid ext format");
                break;
        }

        this->unpack_raw(c.data(), c.size());
    }

    auto unpack_ext() {
        uint8_t f = this->unpack_format();
        std::pair<int8_t, ContainerType> res;

        auto unpackfixext = [&](size_t n) {
            res.first = this->unpack_format();
            res.second.resize(n);
        };

        switch(f) {
            case impl::Format::FIXEXT1: unpackfixext(1); break;
            case impl::Format::FIXEXT2: unpackfixext(2); break;
            case impl::Format::FIXEXT4: unpackfixext(4); break;
            case impl::Format::FIXEXT8: unpackfixext(8); break;

            case impl::Format::EXT8: {
                uint8_t n;
                this->unpack_int(n);
                res.second.resize(n);
                res.first = static_cast<int8_t>(this->unpack_format());
                break;
            }

            case impl::Format::EXT16: {
                uint16_t n;
                this->unpack_int(n);
                res.second.resize(n);
                res.first = static_cast<int8_t>(this->unpack_format());
                break;
            }

            case impl::Format::EXT32: {
                uint32_t n;
                this->unpack_int(n);
                res.second.resize(n);
                res.first = static_cast<int8_t>(this->unpack_format());
                break;
            }

            default:
                impl::msgpack_except(
                    "MsgPack::unpack_int(): Invalid ext format");
                break;
        }

        this->unpack_raw(res.second.data(), res.second.size());
        return res;
    }

    template<typename T>
    Type& unpack(T& t) {
        using U = std::decay_t<T>;

        if constexpr(impl::is_array_v<U>) {
            size_t len = this->unpack_array();

            if constexpr(impl::is_vector_v<U>)
                t.reserve(len);

            for(size_t i = 0; i < len; ++i) {
                typename U::value_type v;
                this->unpack(v);
                if constexpr(impl::is_vector_v<U>)
                    t.push_back(v);
                else
                    t[i] = v;
            }
        }
        else if constexpr(impl::is_map_v<U>) {
            size_t len = this->unpack_map();

            for(size_t i = 0; i < len; ++i) {
                typename U::key_type k;
                typename U::mapped_type v;
                this->unpack(k);
                this->unpack(v);
                t[k] = v;
            }
        }
        else if constexpr(impl::is_string_v<U>)
            this->unpack_string(t);
        else if constexpr(impl::is_bool_v<U>)
            this->unpack_bool(t);
        else if constexpr(std::is_enum_v<U>) {
            std::underlying_type_t<U> u;
            this->unpack_int(u);
            t = static_cast<U>(u);
        }
        else if constexpr(std::is_integral_v<U>)
            this->unpack_int(t);
        else if constexpr(std::is_null_pointer_v<U>) {
            std::uint8_t f = this->unpack_format();
            assert(f == impl::Format::NIL);
            t = nullptr;
        }
        else
            static_assert(impl::always_false_v<U>,
                          "MsgPack::pack(): Unsupported type");

        return *this;
    }

    // Low Level Interface
    Type& pack_map(size_t size) {
        return this->pack_aggregate(
            size,
            {impl::Format::FIXMAP, impl::Format::MAP16, impl::Format::MAP32});
    }
    Type& pack_array(size_t size) {
        return this->pack_aggregate(size, {impl::Format::FIXARRAY,
                                           impl::Format::ARRAY16,
                                           impl::Format::ARRAY32});
    }
    inline size_t unpack_map() {
        return this->unpack_aggregate(
            {impl::Format::FIXMAP, impl::Format::MAP16, impl::Format::MAP32});
    }
    inline size_t unpack_array() {
        return this->unpack_aggregate({impl::Format::FIXARRAY,
                                       impl::Format::ARRAY16,
                                       impl::Format::ARRAY32});
    }

private: // Packing
    inline void pack_bool(bool b) {
        this->pack_format(b ? impl::Format::TRUE : impl::Format::FALSE);
    }

    template<typename T>
    void pack_string(T&& t) {
        using U = std::decay_t<T>;
        const char* p = nullptr;
        size_t sz = 0;

        if constexpr(std::is_same_v<U, std::string> ||
                     std::is_same_v<U, std::string_view>) {
            p = t.data();
            sz = t.size();
        }
        else if constexpr(std::is_same_v<U, const char*>) {
            assert(t);
            p = t;
            sz = std::strlen(t);
        }
        else
            static_assert(impl::always_false_v<U>,
                          "MsgPack::pack_string(): Unsupported string type");

        if(sz <= 31) {
            this->pack_format(impl::Format::FIXSTR | static_cast<uint8_t>(sz));
            this->pack_raw(p, sz);
        }
        else if(sz <= std::numeric_limits<std::uint8_t>::max()) {
            this->pack_format(impl::Format::STR8);
            this->pack_int(static_cast<uint8_t>(sz));
            this->pack_raw(p, sz);
        }
        else if(sz <= std::numeric_limits<uint16_t>::max()) {
            this->pack_format(impl::Format::STR16);
            this->pack_int(static_cast<uint16_t>(sz));
            this->pack_raw(p, sz);
        }
        else if(sz <= std::numeric_limits<uint32_t>::max()) {
            this->pack_format(impl::Format::STR32);
            this->pack_int(static_cast<uint32_t>(sz));
            this->pack_raw(p, sz);
        }
        else
            impl::msgpack_except(
                "MsgPack::pack::string(): Unsupported string size");
    }

    template<typename T,
             typename = std::enable_if_t<std::is_integral_v<std::decay_t<T>>>>
    void pack_int(T t) {
        if constexpr(sizeof(T) > sizeof(uint8_t))
            t = impl::swap_bigendian(t);

        if constexpr(sizeof(T) == sizeof(uint8_t)) {
            if constexpr(std::is_signed_v<T>) {
                if(t < -(1 << 5)) {
                    this->pack_raw(reinterpret_cast<ValueType*>(&t), sizeof(T));
                    return;
                }

                this->pack_format(impl::Format::INT8);
            }
            else {
                if(t < (1 << 7)) {
                    this->pack_raw(reinterpret_cast<ValueType*>(&t), sizeof(T));
                    return;
                }

                this->pack_format(impl::Format::UINT8);
            }
        }
        else if constexpr(sizeof(T) == sizeof(uint16_t))
            this->pack_format(std::is_signed_v<T> ? impl::Format::INT16
                                                  : impl::Format::UINT16);
        else if constexpr(sizeof(T) == sizeof(uint32_t))
            this->pack_format(std::is_signed_v<T> ? impl::Format::INT32
                                                  : impl::Format::UINT32);
        else if constexpr(sizeof(T) == sizeof(uint64_t))
            this->pack_format(std::is_signed_v<T> ? impl::Format::INT64
                                                  : impl::Format::UINT64);
        else
            static_assert(impl::always_false_v<T>,
                          "MsgPack::pack_int(): Unsupported integer type");

        this->pack_raw(reinterpret_cast<const ValueType*>(&t), sizeof(T));
    }

    template<typename T>
    inline void pack_length(T len) {
        len = impl::swap_bigendian(len);
        this->pack_raw(reinterpret_cast<ValueType*>(&len), sizeof(len));
    }

    inline void pack_format(uint8_t f) {
        this->pack_raw(reinterpret_cast<const ValueType*>(&f), sizeof(uint8_t));
    }

    void pack_raw(const ValueType* p, size_t size) {
        assert(p);

        if(size) {
            this->buffer.get().reserve(this->buffer.get().size() + size);
            std::copy_n(p, size, std::back_inserter(this->buffer.get()));
        }
    }

    inline Type& pack_aggregate(size_t size,
                                const std::array<uint8_t, 3>& formats) {
        if(size <= 0xF) {
            this->pack_format(formats[0] | static_cast<uint8_t>(size));
        }
        else if(size <= std::numeric_limits<uint16_t>::max()) {
            this->pack_format(formats[1]);
            this->pack_length(static_cast<uint16_t>(size));
        }
        else if(size <= std::numeric_limits<uint32_t>::max()) {
            this->pack_format(formats[2]);
            this->pack_length(static_cast<uint32_t>(size));
        }
        else
            impl::msgpack_except("MsgPack::new_aggregate(): Unsupported Size");

        return *this;
    }

private: // Unpacking
    size_t unpack_aggregate(const std::array<uint8_t, 3>& formats) {
        uint8_t f = this->unpack_format();
        size_t len = 0;

        if((f & 0xF0) == formats[0]) {
            len = f & 0x0F;
        }
        else if(f == formats[1])
            len = this->unpack_length<uint16_t>();
        else if(f == formats[2])
            len = this->unpack_length<uint32_t>();
        else
            impl::msgpack_except("MsgPack::unpack_aggregate(): Invalid Format");

        return len;
    }

    template<typename T>
    inline T unpack_length() {
        T len = 0;
        this->unpack_raw(reinterpret_cast<ValueType*>(&len), sizeof(T));
        return impl::swap_bigendian(len);
    }

    template<typename T>
    void unpack_string(T& t) {
        uint8_t f = this->unpack_format();
        size_t len = 0;

        if((f & 0xE0) == impl::Format::FIXSTR) {
            len = f & 0x1F;
        }
        else if(f == impl::Format::STR8) {
            uint8_t l;
            this->unpack_int(l);
            len = l;
        }
        else if(f == impl::Format::STR16) {
            uint16_t l;
            this->unpack_int(l);
            len = l;
        }
        else if(f == impl::Format::STR32) {
            uint32_t l;
            this->unpack_int(l);
            len = l;
        }
        else
            impl::msgpack_except("MsgPack::unpack_string(): Invalid Format");

        if constexpr(std::is_same_v<T, std::string>) {
            t.resize(len);
            this->unpack_raw(t.data(), len);
        }
        else if constexpr(std::is_same_v<T, std::string_view>) {
            t = std::string_view{this->buffer.get().data() + this->pos, len};
            this->pos += len;
        }
        else if constexpr(std::is_same_v<T, char const*>)
            this->unpack_raw(&t, len);
        else
            static_assert(impl::always_false_v<T>,
                          "MsgPack::unpack_string(): Invalid type");
    }

    template<typename T,
             typename = std::enable_if_t<std::is_integral_v<std::decay_t<T>>>>
    void unpack_int(T& t) {
        using U = std::decay_t<T>;
        uint8_t f = this->unpack_format();

        switch(f) {
            case impl::Format::INT8: assert((std::is_same_v<U, int8_t>)); break;
            case impl::Format::INT16:
                assert((std::is_same_v<U, int16_t>));
                break;
            case impl::Format::INT32:
                assert((std::is_same_v<U, int32_t>));
                break;
            case impl::Format::INT64:
                assert((std::is_same_v<U, int64_t>));
                break;
            case impl::Format::UINT8:
                assert((std::is_same_v<U, uint8_t>));
                break;
            case impl::Format::UINT16:
                assert((std::is_same_v<U, uint16_t>));
                break;
            case impl::Format::UINT32:
                assert((std::is_same_v<U, uint32_t>));
                break;
            case impl::Format::UINT64:
                assert((std::is_same_v<U, uint64_t>));
                break;

            default: {
                int sel = static_cast<int>(f);

                if(sel >= 0x00 && sel <= 0x7F) { // Positive FIXNUM
                    t = static_cast<T>(static_cast<uint8_t>(f));
                    return;
                }

                if(sel >= 0xe0 && sel <= 0xFF) { // Negative FIXNUM
                    t = static_cast<T>(static_cast<int8_t>(f));
                    return;
                }

                impl::msgpack_except(
                    "MsgPack::unpack_int(): Invalid integer format");
                break;
            }
        }

        this->unpack_raw(reinterpret_cast<ValueType*>(&t), sizeof(U));
        if constexpr(sizeof(U) > sizeof(uint8_t))
            t = impl::swap_bigendian(t);
    }

    void unpack_bool(bool& b) {
        uint8_t f = this->unpack_format();
        if(f == impl::Format::TRUE)
            b = true;
        else if(f == impl::Format::FALSE)
            b = false;
        else
            impl::msgpack_except("MsgPack::unpack_bool(): Invalid Format");
    }

    uint8_t unpack_format() {
        uint8_t f;
        this->unpack_raw(reinterpret_cast<ValueType*>(&f), sizeof(uint8_t));
        return f;
    }

    void unpack_raw(ValueType* p, size_t size) {
        assert(p);

        if(size) {
            auto endpos = this->pos + size;
            if(endpos > this->buffer.get().size())
                impl::msgpack_except("MsgPack::unpack_raw(): Reached EOB");
            std::copy_n(this->buffer.get().begin() + this->pos, size, p);
            this->pos = endpos;
        }
    }

public:
    std::reference_wrapper<Container> buffer;
    bool m_readonly{false};
    size_t pos{};
};

using MsgPack = BasicMsgPack<std::string>;
using Visitor = BasicVisitor<std::string>;

template<typename MsgPackType = MsgPack, typename VisitorType>
VisitorType& visit(const typename MsgPackType::ContainerType& c,
                   VisitorType&& visitor) {
    MsgPackType mp{c};

    while(mp.pos < c.size()) {
        if(!impl::visit(mp, std::forward<VisitorType>(visitor)))
            break;
    }

    return visitor;
}

template<typename T, typename MsgPackType = MsgPack>
typename MsgPackType::ContainerType pack(T&& t) {
    typename MsgPackType::ContainerType c;
    MsgPackType mp{c};
    mp.pack(std::forward<T>(t));
    return c;
}

template<typename T, typename MsgPackType = MsgPack>
void unpack(typename MsgPackType::ContainerType& buffer, T& t) {
    MsgPackType mp{buffer};
    mp.unpack(t);
}

} // namespace msgpack

#if defined(MSGPACK_NAMESPACE)
} // namespace MSGPACK_NAMESPACE
#endif
