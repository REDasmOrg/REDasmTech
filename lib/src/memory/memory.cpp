#include "memory.h"
#include "../state.h"

namespace redasm {

namespace memory {

namespace {

tl::optional<std::pair<MIndex, MIndex>> find_range(const RDBuffer* self,
                                                   usize idx) {
    assume(self);
    if(idx >= self->len) return tl::nullopt;

    // Single item range
    if(rdbyte_hasflag(&self->m_data[idx], BF_START | BF_END))
        return std::make_pair(idx, idx);

    usize rstart = idx, rend = idx;

    // Traverse backward to find the range start
    while(rstart > 0 && rdbyte_hasflag(&self->m_data[rstart], BF_CONT))
        rstart--;

    if(rstart >= self->len || !rdbyte_hasflag(&self->m_data[rstart], BF_START))
        return tl::nullopt; // Not part of a valid range

    // Traverse forward to find the range end
    while(rend < self->len || (rdbyte_hasflag(&self->m_data[rend], BF_START) ||
                               (rdbyte_hasflag(&self->m_data[rend], BF_CONT) &&
                                !rdbyte_hasflag(&self->m_data[rend], BF_END))))
        rend++;

    if(rend >= self->len || !rdbyte_hasflag(&self->m_data[rend], BF_END))
        return tl::nullopt; // Not part of a valid range

    assume(rstart <= rend);
    return std::make_pair(rstart, rend);
}

tl::optional<std::string> get_str_impl(const RDSegmentNew* self,
                                       RDAddress address,
                                       typing::TypeName tname) {
    if(address >= self->end) return tl::nullopt;

    const typing::TypeDef* t = state::get_types().get_typedef(tname);
    assume(t);

    std::string s;

    for(RDAddress addr = address; addr < self->end; addr += t->size) {
        if(auto v = memory::get_type(self, addr, tname); v) {
            if(!v->ch_v) // Stop at terminator
                break;

            s += v->ch_v;
        }
        else
            break;
    }

    return s;
}

tl::optional<std::string> get_str_impl(const RDSegmentNew* self,
                                       RDAddress address, usize n,
                                       std::string_view tname) {
    std::string s;
    s.reserve(n);

    const typing::TypeDef* t = state::get_types().get_typedef(tname);
    assume(t);

    const usize ENDADDR = std::min(address + n, self->end);

    for(RDAddress addr = address; addr < ENDADDR; addr += t->size) {
        if(auto v = memory::get_type(self, addr, tname); v)
            s += v->ch_v;
        else
            break;
    }

    return s;
}

tl::optional<RDValue> get_type_impl(const RDSegmentNew* self,
                                    RDAddress& address, RDType t);

tl::optional<RDValue> get_type_impl(const RDSegmentNew* self,
                                    RDAddress& address,
                                    const typing::TypeDef* tdef) {
    RDValue v = rdvalue_create();
    v.type = tdef->to_type();

    if(tdef->is_struct()) {
        for(const auto& [t, n] : tdef->dict) {
            if(auto item = memory::get_type_impl(self, address, t); item)
                dict_set(RDValueField, &v.dict, str_create(n.c_str()), *item);
            else
                return tl::nullopt;
        }
    }
    else {
        switch(tdef->get_id()) {
            case typing::ids::BOOL: {
                if(auto b = memory::get_bool(self, address); b) {
                    v.b_v = *b;
                    address += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::CHAR: {
                if(auto b = memory::get_char(self, address); b) {
                    v.ch_v = *b;
                    address += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::WCHAR: {
                if(auto b = memory::get_wchar(self, address); b) {
                    v.ch_v = *b;
                    address += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::U8: {
                if(auto b = memory::get_u8(self, address); b) {
                    v.u8_v = *b;
                    address += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::U16: {
                if(auto b = memory::get_u16(self, address, tdef->is_big()); b) {
                    v.u16_v = *b;
                    address += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::U32: {
                if(auto b = memory::get_u32(self, address, tdef->is_big()); b) {
                    v.u32_v = *b;
                    address += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::U64: {
                if(auto b = memory::get_u64(self, address, tdef->is_big()); b) {
                    v.u64_v = *b;
                    address += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::I8: {
                if(auto b = memory::get_i8(self, address); b) {
                    v.i8_v = *b;
                    address += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::I16: {
                if(auto b = memory::get_i16(self, address, tdef->is_big()); b) {
                    v.i16_v = *b;
                    address += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::I32: {
                if(auto b = memory::get_i32(self, address, tdef->is_big()); b) {
                    v.i32_v = *b;
                    address += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::I64: {
                if(auto b = memory::get_i64(self, address, tdef->is_big()); b) {
                    v.i64_v = *b;
                    address += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::STR: {
                if(auto s = memory::get_str(self, address); s) {
                    v.str = str_create_n(s->c_str(), 0, s->size());
                    address += s->size();
                }
                else
                    return tl::nullopt;
                break;
            }

            case typing::ids::WSTR: {
                if(auto s = memory::get_wstr(self, address); s) {
                    v.str = str_create_n(s->c_str(), 0, s->size());
                    address += s->size();
                }
                else
                    return tl::nullopt;
                break;
            }

            default: unreachable;
        }
    }

    return v;
}

tl::optional<RDValue> get_type_impl(const RDSegmentNew* self,
                                    RDAddress& address, RDType t) {
    const typing::TypeDef* td = state::get_types().get_typedef(t);
    assume(td);

    if(t.n > 0) {
        RDValue v = rdvalue_create();
        v.type = t;
        vect_reserve(&v.list, t.n);

        for(usize i = 0; i < t.n; i++) {
            auto item = memory::get_type_impl(self, address, td);
            if(item)
                vect_append(RDValue, &v.list, *item);
            else
                return tl::nullopt;
        }

        return v;
    }

    return memory::get_type_impl(self, address, td);
}

} // namespace

usize get_length(const RDSegmentNew* self, RDAddress address) {
    if(auto r = memory::find_range(&self->mem, address - self->start); r)
        return r->second - r->first + 1;
    return 0;
}

tl::optional<RDAddress> get_next(const RDSegmentNew* self, RDAddress address) {
    if(address >= self->end) return tl::nullopt;

    if(usize len = memory::get_length(self, address); len > 0)
        return address + len;
    return tl::nullopt;
}

bool is_unknown(const RDSegmentNew* self, RDAddress address) {
    return rdbyte_isunknown(&self->mem.m_data[address - self->start]);
}

bool has_common(const RDSegmentNew* self, RDAddress address) {
    return rdbyte_hascommon(&self->mem.m_data[address - self->start]);
}

bool has_byte(const RDSegmentNew* self, RDAddress address) {
    return rdbyte_hasbyte(&self->mem.m_data[address - self->start]);
}

bool has_flag(const RDSegmentNew* self, RDAddress address, u32 f) {
    return rdbyte_hasflag(&self->mem.m_data[address - self->start], f);
}

void set_flag(RDSegmentNew* self, RDAddress address, u32 f, bool b) {
    rdbyte_setflag(&self->mem.m_data[address - self->start], f, b);
}

void clear(RDSegmentNew* self, RDAddress address) {
    rdbyte_clear(&self->mem.m_data[address - self->start]);
}

void set_n(RDSegmentNew* self, RDAddress address, usize n, u32 flags) {
    assume(self);
    usize idx = address - self->start;
    if(idx >= self->mem.len || !n) return;

    usize end = std::min(idx + n, self->mem.len);
    memory::set_flag(self, idx, flags | BF_START);

    for(usize i = idx + 1; i < end - 1; i++)
        memory::set_flag(self, i, flags | BF_CONT);

    if(n > 1) memory::set_flag(self, end - 1, flags | BF_CONT);
    memory::set_flag(self, end - 1, flags | BF_END);
}

void unset_n(RDSegmentNew* self, RDAddress address, usize n) {
    assume(self);
    usize idx = address - self->start;
    usize end = std::min(idx + n, self->mem.len);

    for(usize i = idx; i < end; i++) {
        if(memory::has_flag(self, i, BF_START) ||
           memory::has_flag(self, i, BF_CONT)) {
            auto r = memory::find_range(&self->mem, i);
            assume(r);

            // Unset the overlapping range
            for(MIndex j = r->first; j <= r->second; j++)
                memory::clear(self, j);

            // Skip to the end of the cleared range
            i = r->second;
        }
        else
            memory::clear(self, i);
    }
}

RDByte get_mbyte(const RDSegmentNew* self, RDAddress address) {
    usize idx = address - self->start;
    if(idx < self->mem.len) return self->mem.m_data[idx];
    except("memory::get_byte(): address out of range");
}

tl::optional<u8> get_byte(const RDSegmentNew* self, RDAddress address) {
    u8 b;
    if((address >= self->start && address < self->end) &&
       self->mem.get_byte(&self->mem, address - self->start, &b))
        return b;

    return tl::nullopt;
}

tl::optional<RDValue> get_type(const RDSegmentNew* self, RDAddress address,
                               typing::FullTypeName tn) {
    typing::ParsedType pt = state::get_types().parse(tn);
    return memory::get_type(self, address, pt.to_type());
}

tl::optional<RDValue> get_type(const RDSegmentNew* self, RDAddress address,
                               RDType t) {
    return memory::get_type_impl(self, address, t);
}

tl::optional<std::string> get_str(const RDSegmentNew* self, RDAddress address) {
    return memory::get_str_impl(self, address, typing::names::CHAR);
}

tl::optional<std::string> get_str(const RDSegmentNew* self, RDAddress address,
                                  usize n) {
    return memory::get_str_impl(self, address, n, typing::names::CHAR);
}

tl::optional<std::string> get_wstr(const RDSegmentNew* self,
                                   RDAddress address) {
    return memory::get_str_impl(self, address, typing::names::WCHAR);
}

tl::optional<std::string> get_wstr(const RDSegmentNew* self, RDAddress address,
                                   usize n) {
    return memory::get_str_impl(self, address, n, typing::names::WCHAR);
}

tl::optional<bool> get_bool(const RDSegmentNew* self, RDAddress address) {
    auto b = memory::get_byte(self, address);
    if(b) return !!(*b);
    return tl::nullopt;
}

tl::optional<char> get_char(const RDSegmentNew* self, RDAddress address) {
    auto b = memory::get_byte(self, address);
    if(b) return static_cast<char>(*b);
    return tl::nullopt;
}

tl::optional<char> get_wchar(const RDSegmentNew* self, RDAddress address) {
    auto b = memory::get_u16(self, address, false);
    if(b) return static_cast<char>(*b & 0xFF);
    return tl::nullopt;
}

} // namespace memory

tl::optional<u8> Memory::get_byte(usize idx) const {
    if(idx >= m_buffer.size()) return tl::nullopt;

    Byte b = m_buffer[idx];
    if(b.has_byte()) return b.byte();
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
    if(idx < m_buffer.size()) m_buffer.at(idx).set_flag(flags, b);
}

void Memory::set_n(MIndex idx, usize len, u32 flags) {
    if(idx >= m_buffer.size() || !len) return;

    MIndex end = std::min(idx + len, m_buffer.size());
    m_buffer[idx].set(flags | BF_START);

    for(MIndex i = idx + 1; i < end - 1; i++)
        m_buffer[i].set(flags | BF_CONT);

    if(len > 1) m_buffer[end - 1].set(flags | BF_CONT);

    m_buffer[end - 1].set(flags | BF_END);
}

usize Memory::get_length(MIndex idx) const {
    if(auto r = this->find_range(idx); r) return r->second - r->first + 1;
    return 0;
}

tl::optional<MIndex> Memory::get_next(MIndex idx) const {
    if(idx >= this->size()) return tl::nullopt;
    if(usize len = this->get_length(idx); len > 0) return idx + len;
    return tl::nullopt;
}

tl::optional<std::pair<MIndex, MIndex>> Memory::find_range(MIndex idx) const {
    if(idx >= m_buffer.size()) return tl::nullopt;

    // Single item range
    if(m_buffer[idx].has(BF_START | BF_END)) return std::make_pair(idx, idx);

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
