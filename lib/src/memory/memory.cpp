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
    MIndex end = std::min(idx + len, m_buffer.size());

    for(MIndex i = idx; i < end; i++) {
        if(m_buffer[i].is_start() || m_buffer[i].is_cont()) {
            auto r = this->find_range(i);
            assume(r);

            // Unset the overlapping range
            for(MIndex j = r->first; j <= r->second; j++)
                m_buffer[j].clear();

            // Skip to the end of the cleared range
            i = r->second;
        }
        else
            m_buffer[i].clear();
    }
}

void Memory::set_flags(MIndex idx, u32 flags, bool b) {
    if(idx < m_buffer.size())
        m_buffer.at(idx).set_flag(flags, b);
}

void Memory::set_n(MIndex idx, usize len, u32 flags) {
    if(idx >= m_buffer.size() || !len)
        return;

    MIndex end = std::min(idx + len, m_buffer.size());
    m_buffer[idx].set(flags | BF_START);

    for(MIndex i = idx + 1; i < end - 1; i++)
        m_buffer[i].set(flags | BF_CONT);

    if(len > 1)
        m_buffer[end - 1].set(flags | BF_CONT);

    m_buffer[end - 1].set(flags | BF_END);
}

usize Memory::get_length(MIndex idx) const {
    if(auto r = this->find_range(idx); r)
        return r->second - r->first + 1;

    return 0;
}

tl::optional<MIndex> Memory::get_next(MIndex idx) const {
    if(idx >= this->size())
        return tl::nullopt;

    if(usize len = this->get_length(idx); len > 0)
        return idx + len;

    return tl::nullopt;
}

tl::optional<std::pair<MIndex, MIndex>> Memory::find_range(MIndex idx) const {
    if(idx >= m_buffer.size())
        return tl::nullopt;

    // Single item range
    if(m_buffer[idx].has(BF_START | BF_END))
        return std::make_pair(idx, idx);

    usize rstart = idx, rend = idx;

    // Traverse backward to find the range start
    while(rstart > 0 && m_buffer[rstart].is_cont())
        rstart--;

    if(rstart >= m_buffer.size() || !m_buffer[rstart].is_start())
        return tl::nullopt; // Not part of a valid range

    // Traverse forward to find the range end
    while(rend < m_buffer.size() &&
          (m_buffer[rend].is_start() ||
           (m_buffer[rend].is_cont() && !m_buffer[rend].is_end())))
        rend++;

    if(rend >= m_buffer.size() || !m_buffer[rend].is_end())
        return tl::nullopt; // Not part of a valid range

    assume(rstart <= rend);
    return std::make_pair(rstart, rend);
}

} // namespace redasm
