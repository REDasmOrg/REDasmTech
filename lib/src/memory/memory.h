#pragma once

#include "abstractbuffer.h"
#include "byte.h"
#include <climits>
#include <tl/optional.hpp>

namespace redasm {

class Memory: public AbstractBufferT<Byte> {
public:
    explicit Memory(usize sz): AbstractBufferT<Byte>{{}} {
        m_buffer.resize(sz);
    }

    tl::optional<u8> get_byte(usize idx) const override;
    usize get_length(MIndex idx) const;
    tl::optional<MIndex> get_next(MIndex idx) const;
    void set(MIndex idx, u32 flags);
    void set_n(MIndex idx, usize len, u32 flags);
    void unset_n(MIndex idx, usize len);
    void set_flags(MIndex idx, u32 flags, bool b);

private:
    tl::optional<std::pair<MIndex, MIndex>> find_range(MIndex idx) const;
};

} // namespace redasm
