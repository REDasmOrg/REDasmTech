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
};

} // namespace redasm
