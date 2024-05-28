#include "file.h"

namespace redasm {

[[nodiscard]] tl::optional<u8> File::get_byte(usize idx) const {
    if(idx >= m_buffer.size())
        return tl::nullopt;
    return m_buffer[idx];
}

} // namespace redasm
