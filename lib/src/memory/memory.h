#pragma once

#include "abstractbuffer.h"
#include "byte.h"
#include "byteorder.h"
#include <climits>
#include <redasm/segment.h>
#include <tl/optional.hpp>

namespace redasm {

namespace memory {

tl::optional<u8> get_byte(const RDSegmentNew* self, RDAddress address);

namespace impl {

template<typename T>
tl::optional<T> get_number(const RDSegmentNew* self, RDAddress address,
                           bool big) {
    static constexpr usize N = sizeof(T);
    T num = 0;

    for(usize i = 0; i < N; i++) {
        if(auto b = memory::get_byte(self, address + i); b)
            num |= (static_cast<T>(*b) << (i * CHAR_BIT));
        else
            return tl::nullopt;
    }

    if constexpr(sizeof(T) == sizeof(u8))
        return num;
    else {
        if(big) return byteorder::to_bigendian(num);
        return byteorder::to_littleendian(num);
    }
}

} // namespace impl

usize get_length(const RDSegmentNew* self, RDAddress address);
tl::optional<RDAddress> get_next(const RDSegmentNew* self, RDAddress address);
bool is_unknown(const RDSegmentNew* self, RDAddress address);
bool has_common(const RDSegmentNew* self, RDAddress address);
bool has_byte(const RDSegmentNew* self, RDAddress address);
bool has_flag(const RDSegmentNew* self, RDAddress address, u32 f);
void set_flag(RDSegmentNew* self, RDAddress address, u32 f, bool b = true);
void clear(RDSegmentNew* self, RDAddress address);
void set_n(RDSegmentNew* self, RDAddress address, usize n, u32 flags);
void unset_n(RDSegmentNew* self, RDAddress address, usize n);
RDByte get_mbyte(const RDSegmentNew* self, RDAddress address);
tl::optional<RDValue> get_type(const RDSegmentNew* self, RDAddress address,
                               typing::FullTypeName tn);
tl::optional<RDValue> get_type(const RDSegmentNew* self, RDAddress address,
                               RDType t);
tl::optional<std::string> get_str(const RDSegmentNew* self, RDAddress address);
tl::optional<std::string> get_str(const RDSegmentNew* self, RDAddress address,
                                  usize n);
tl::optional<std::string> get_wstr(const RDSegmentNew* self, RDAddress address);
tl::optional<std::string> get_wstr(const RDSegmentNew* self, RDAddress address,
                                   usize n);
tl::optional<bool> get_bool(const RDSegmentNew* self, RDAddress address);
tl::optional<char> get_char(const RDSegmentNew* self, RDAddress address);
tl::optional<char> get_wchar(const RDSegmentNew* self, RDAddress address);

inline tl::optional<u8> get_u8(const RDSegmentNew* self, RDAddress address) {
    return memory::get_byte(self, address);
}

inline tl::optional<u16> get_u16(const RDSegmentNew* self, RDAddress address,
                                 bool big) {
    return impl::get_number<u16>(self, address, big);
}

inline tl::optional<u32> get_u32(const RDSegmentNew* self, RDAddress address,
                                 bool big) {
    return impl::get_number<u32>(self, address, big);
}

inline tl::optional<u64> get_u64(const RDSegmentNew* self, RDAddress address,
                                 bool big) {
    return impl::get_number<u64>(self, address, big);
}

inline tl::optional<i8> get_i8(const RDSegmentNew* self, RDAddress address) {
    if(auto b = memory::get_u8(self, address); b) return static_cast<i8>(*b);
    return tl::nullopt;
}

inline tl::optional<i16> get_i16(const RDSegmentNew* self, RDAddress address,
                                 bool big) {
    return impl::get_number<i16>(self, address, big);
}

inline tl::optional<i32> get_i32(const RDSegmentNew* self, RDAddress address,
                                 bool big) {
    return impl::get_number<i32>(self, address, big);
}

inline tl::optional<i64> get_i64(const RDSegmentNew* self, RDAddress address,
                                 bool big) {
    return impl::get_number<i64>(self, address, big);
}

template<typename Function>
bool range_is(const RDSegmentNew* self, RDAddress address, usize n,
              Function f) {
    RDAddress endaddr = std::min(address + n, self->end);

    for(RDAddress i = address; i < endaddr; i++) {
        auto b = memory::get_byte(self, address);
        if(!b || !f(*b)) return false;
    }

    return true;
}

} // namespace memory

class Memory: public AbstractBufferT<Byte> {
public:
    Memory(): AbstractBufferT<Byte>{{}} {}

    explicit Memory(usize sz): AbstractBufferT<Byte>{{}} {
        m_buffer.resize(sz);
    }

    tl::optional<u8> get_byte(usize idx) const override;
    usize get_length(MIndex idx) const;
    tl::optional<MIndex> get_next(MIndex idx) const;
    void set_n(MIndex idx, usize len, u32 flags);
    void unset_n(MIndex idx, usize len);
    void set_flags(MIndex idx, u32 flags, bool b);

    void set(MIndex idx, u32 flags) { this->set_flags(idx, flags, true); }
    void unset(MIndex idx, u32 flags) { this->set_flags(idx, flags, false); }

    template<typename Function>
    bool range_is(MIndex idx, usize len, Function f) const {
        MIndex endidx = std::min(idx + len, this->size());

        for(MIndex i = idx; i < endidx; i++) {
            if(!f(this->at(i))) return false;
        }

        return true;
    }

private:
    tl::optional<std::pair<MIndex, MIndex>> find_range(MIndex idx) const;
};

} // namespace redasm
