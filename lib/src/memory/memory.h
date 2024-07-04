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
    usize get_length(MIndex idx) const;
    tl::optional<MIndex> get_next(MIndex idx) const;
    void set(MIndex idx, usize len, u32 flags);
    void unset(MIndex idx, usize len);
    void unset(MIndex idx);

    inline void set_unknown(MIndex idx, usize len) {
        this->set(idx, len, BF_UNKNOWN);
    }

    inline void set_data(MIndex idx, usize len) {
        this->set(idx, len, BF_DATA);
    }
    inline void set_code(MIndex idx, usize len) {
        this->set(idx, len, BF_CODE);
    }
};

} // namespace redasm
