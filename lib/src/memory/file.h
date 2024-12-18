#pragma once

#include "abstractbuffer.h"

namespace redasm {

class File: public AbstractBufferT<u8> {
public:
    explicit File(std::string src): AbstractBufferT<u8>{std::move(src)} {}
    const typename Type::value_type* data() const { return m_buffer.data(); }
    typename Type::value_type* data() { return m_buffer.data(); }
    [[nodiscard]] tl::optional<u8> get_byte(usize idx) const override;
};

} // namespace redasm
