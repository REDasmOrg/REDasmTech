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

void Memory::unset(MIndex idx, usize len) {
    for(MIndex i = idx; i < idx + len && i < m_buffer.size(); i++)
        m_buffer[i].value &= BF_MMASK; // Clear Specific Flags
}

void Memory::unset(MIndex idx) {
    u32 c = m_buffer[idx].category();
    if(c == BF_UNKNOWN)
        return;

    for(MIndex i = idx; i < m_buffer.size(); i++) {
        Byte& b = m_buffer[i];

        if(b.category() != c || (i > idx && !b.is_cont()))
            break;

        m_buffer[i].value &= BF_MMASK; // Clear Specific Flags
    }
}

void Memory::set(MIndex idx, usize len, u32 flags) {
    MIndex endlen = std::min(idx + len, m_buffer.size());

    for(MIndex i = idx; i < endlen; i++) {
        m_buffer[i].set(flags);
        flags |= BF_CONT;
    }
}

usize Memory::get_length(MIndex idx) const {
    usize c = 0;

    for(; idx < this->size() && this->at(idx).is_cont(); c++)
        idx++;

    return c;
}

tl::optional<MIndex> Memory::get_next(MIndex idx) const {
    if(idx >= this->size())
        return tl::nullopt;

    return idx + this->get_length(idx);
}

} // namespace redasm
