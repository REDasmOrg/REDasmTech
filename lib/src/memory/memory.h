#pragma once

#include "abstractbuffer.h"
#include "byte.h"
#include <climits>

namespace redasm {

class Memory: public AbstractBufferT<Byte> {
public:
    explicit Memory(usize sz): AbstractBufferT<Byte>{{}} {
        m_buffer.resize(sz);
    }

    tl::optional<u8> get_byte(usize idx) const override;
    usize get_length(usize idx) const;
    void set(usize idx, usize len, u32 flags);
    void unset(usize idx, usize len);
    void unset(usize idx);

    inline void set_unknown(usize idx, usize len) {
        this->set(idx, len, BF_UNKNOWN);
    }

    inline void set_data(usize idx, usize len) { this->set(idx, len, BF_DATA); }
    inline void set_code(usize idx, usize len) { this->set(idx, len, BF_CODE); }
};

} // namespace redasm
