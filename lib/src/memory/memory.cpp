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

void Memory::set_unknown(usize idx, usize len) {
    usize endlen = std::min(idx + len, m_buffer.size());

    for(usize i = idx; i < endlen; i++)
        m_buffer[i].value &= BF_BYTEVMASK;
}

void Memory::set_data(usize idx, usize len) { this->set(idx, len, BF_DATA); }
void Memory::set_code(usize idx, usize len) { this->set(idx, len, BF_CODE); }

void Memory::set(usize idx, usize len, u32 flags) {
    usize endlen = std::min(idx + len, m_buffer.size());

    for(usize i = idx; i < endlen; i++) {
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
