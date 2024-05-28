#pragma once

#include "abstractbuffer.h"
#include "byte.h"
#include <climits>

namespace redasm {

class Memory: public AbstractBufferT<Byte> {
public:
    Memory() = default;
    explicit Memory(usize sz) { m_buffer.resize(sz); }
    tl::optional<u8> get_byte(usize idx) const override;
    void set_unknown(usize idx, usize len);
    void set_data(usize idx, usize len);
    void set_code(usize idx, usize len);
    usize get_length(usize idx) const;

private:
    void set(usize idx, usize len, u32 flags);
};

} // namespace redasm
