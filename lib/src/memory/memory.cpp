#include "memory.h"
#include "../error.h"
#include "../state.h"
#include "mbyte.h"

namespace redasm::memory {

namespace {

tl::optional<std::pair<RDAddress, RDAddress>> find_range(const RDSegment* self,
                                                         RDAddress addr) {
    assume(self);
    if(addr >= self->end) return tl::nullopt;

    // Single item range
    if(memory::has_flag(self, addr, BF_START | BF_END))
        return std::make_pair(addr, addr);

    RDAddress rstart = addr, rend = addr;

    // Traverse backward to find the range start
    while(rstart > 0 && memory::has_flag(self, rstart, BF_CONT))
        rstart--;

    if(rstart >= self->end || !memory::has_flag(self, rstart, BF_START))
        return tl::nullopt; // Not part of a valid range

    // Traverse forward to find the range end
    while(rend < self->end && (memory::has_flag(self, rend, BF_START) ||
                               (memory::has_flag(self, rend, BF_CONT) &&
                                !memory::has_flag(self, rend, BF_END)))) {
        rend++;
    }

    if(rend >= self->end || !memory::has_flag(self, rend, BF_END))
        return tl::nullopt; // Not part of a valid range

    assume(rstart <= rend);
    return std::make_pair(rstart, rend);
}

tl::optional<RDValue> get_type_impl(const RDSegment* self, RDAddress& address,
                                    RDType t);

tl::optional<RDValue> get_type_impl(const RDSegment* self, RDAddress& address,
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

tl::optional<RDValue> get_type_impl(const RDSegment* self, RDAddress& address,
                                    RDType t) {
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

usize get_length(const RDSegment* self, RDAddress address) {
    if(auto r = memory::find_range(self, address); r)
        return r->second - r->first + 1;
    return 0;
}

tl::optional<RDAddress> get_next(const RDSegment* self, RDAddress address) {
    if(address >= self->end) return tl::nullopt;

    if(usize len = memory::get_length(self, address); len > 0)
        return address + len;
    return tl::nullopt;
}

bool is_unknown(const RDSegment* self, RDAddress address) {
    return mbyte::is_unknown(self->mem->m_data[address - self->start]);
}

bool has_common(const RDSegment* self, RDAddress address) {
    return mbyte::has_common(self->mem->m_data[address - self->start]);
}

bool has_byte(const RDSegment* self, RDAddress address) {
    return mbyte::has_byte(self->mem->m_data[address - self->start]);
}

bool has_flag(const RDSegment* self, RDAddress address, u32 f) {
    return mbyte::has_flag(self->mem->m_data[address - self->start], f);
}

void set_flag(RDSegment* self, RDAddress address, u32 f, bool b) {
    mbyte::set_flag(&self->mem->m_data[address - self->start], f, b);
}

void clear(RDSegment* self, RDAddress address) {
    mbyte::clear(&self->mem->m_data[address - self->start]);
}

void set_n(RDSegment* self, RDAddress address, usize n, u32 flags) {
    assume(self);
    if(!n) return;

    RDAddress end = std::min(address + n, self->end);
    memory::set_flag(self, address, flags | BF_START);

    for(RDAddress addr = address + 1; addr < end - 1; addr++)
        memory::set_flag(self, addr, flags | BF_CONT);

    if(n > 1) memory::set_flag(self, end - 1, flags | BF_CONT);
    memory::set_flag(self, end - 1, flags | BF_END);
}

void unset_n(RDSegment* self, RDAddress address, usize n) {
    assume(self);
    RDAddress end = std::min(address + n, self->end);

    for(RDAddress addr = address; addr < end; addr++) {
        if(memory::has_flag(self, addr, BF_START) ||
           memory::has_flag(self, addr, BF_CONT)) {
            auto r = memory::find_range(self, addr);
            assume(r);

            // Unset the overlapping range
            for(RDAddress raddr = r->first; raddr <= r->second; raddr++)
                memory::clear(self, raddr);

            // Skip to the end of the cleared range
            addr = r->second;
        }
        else
            memory::clear(self, addr);
    }
}

RDMByte get_mbyte(const RDSegment* self, RDAddress address) {
    usize idx = address - self->start;
    if(idx < self->mem->length) return self->mem->m_data[idx];
    except("memory::get_mbyte(): address out of range");
}

} // namespace redasm::memory
