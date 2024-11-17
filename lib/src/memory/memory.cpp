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

void Memory::unset_n(MIndex idx, usize len) {
    for(MIndex i = idx; i < idx + len && i < m_buffer.size(); i++)
        m_buffer[i].value &= BF_MMASK; // Clear Specific Flags
}

void Memory::unset(MIndex idx) {
    u32 c = m_buffer[idx].category();
    if(c == BF_UNKNOWN)
        return;

    for(MIndex i = idx; i < m_buffer.size(); i++) {
        Byte oldb = m_buffer[i];

        if(oldb.category() != c)
            break;

        m_buffer[i].value &= BF_MMASK; // Clear Specific Flags

        if(!oldb.is_cont())
            break;
    }
}

void Memory::set_flags(MIndex idx, u32 flags, bool b) {
    if(idx < m_buffer.size())
        m_buffer.at(idx).set_flag(flags, b);
}

void Memory::set(MIndex idx, u32 flags) {
    if(idx < m_buffer.size())
        m_buffer.at(idx).set(flags);
}

void Memory::set_n(MIndex idx, usize len, u32 flags) {
    if(flags != BF_UNKNOWN)
        flags |= BF_CONT; // Unknown doesn't need CONT bit

    MIndex endlen = std::min(idx + len, m_buffer.size());

    for(MIndex i = idx; i < endlen; i++) {
        if(i == endlen - 1)
            flags &= ~BF_CONT;

        m_buffer[i].set(flags);
    }
}

usize Memory::get_length(MIndex startidx) const {
    usize idx = startidx;

    while(idx < this->size()) {
        if(!this->at(idx++).is_cont())
            break;
    }

    return idx - startidx;
}

tl::optional<MIndex> Memory::get_next(MIndex idx) const {
    if(idx >= this->size())
        return tl::nullopt;

    if(usize len = this->get_length(idx); len > 0)
        return idx + len;

    return tl::nullopt;
}

} // namespace redasm
