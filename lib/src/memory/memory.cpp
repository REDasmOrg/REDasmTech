#include "memory.h"

namespace redasm {

tl::optional<u8> Memory::get_byte(usize idx) const {
    if(idx >= m_buffer.size())
        return tl::nullopt;

    Byte b = m_buffer[idx];

    if(b.has_byte())
        return b.byte();

    return tl::nullopt;
}

void Memory::unset(usize idx) {
    u32 c = m_buffer[idx].category();
    if(c == BF_UNKNOWN)
        return;

    for(usize i = idx; i < m_buffer.size(); i++) {
        Byte& b = m_buffer[i];

        if(b.category() != c || (i > idx && !b.is_cont()))
            break;

        m_buffer[i].value &= BF_MMASK; // Clear Specific Flags
    }
}

void Memory::set(usize idx, usize len, u32 flags) {
    usize endlen = std::min(idx + len, m_buffer.size());

    for(usize i = idx; i < endlen; i++) {
        m_buffer[i].value &= BF_MMASK; // Clear Specific Flags
        m_buffer[i].set(flags);
        flags |= BF_CONT;
    }
}

usize Memory::get_length(usize idx) const {
    usize c = 0;

    for(; idx < this->size() && this->at(idx).is_cont(); c++)
        idx++;

    return c;
}

} // namespace redasm
