#include "buffer.h"
#include "../error.h"
#include "../state.h"
#include <redasm/types.h>

namespace redasm::buffer {

namespace {

tl::optional<std::string> get_str_impl(const RDBuffer* self, usize idx,
                                       typing::TypeName tname) {
    if(idx >= self->length) return tl::nullopt;

    const typing::TypeDef* t = state::get_types().get_typedef(tname);
    assume(t);

    std::string s;

    for(usize curr = idx; curr < self->length; curr += t->size) {
        if(auto v = buffer::get_type(self, curr, tname); v) {
            if(!v->ch_v) // Stop at terminator
                break;
            s += v->ch_v;
        }
        else
            break;
    }

    return s;
}

tl::optional<std::string> get_str_impl(const RDBuffer* self, usize idx, usize n,
                                       std::string_view tname) {
    std::string s;
    s.reserve(n);

    const typing::TypeDef* t = state::get_types().get_typedef(tname);
    assume(t);

    const usize ENDADDR = std::min(idx + n, self->length);

    for(usize curr = idx; curr < ENDADDR; curr += t->size) {
        if(auto v = buffer::get_type(self, curr, tname); v)
            s += v->ch_v;
        else
            break;
    }

    return s;
}

tl::optional<RDValue> get_type_impl(const RDBuffer* self, usize& idx, RDType t);

tl::optional<RDValue> get_type_impl(const RDBuffer* self, usize& idx,
                                    const typing::TypeDef* tdef) {
    RDValue v = rdvalue_create();
    v.type = tdef->to_type();

    if(tdef->is_struct()) {
        for(const auto& [t, n] : tdef->dict) {
            if(auto item = buffer::get_type_impl(self, idx, t); item)
                dict_set(RDValueField, &v.dict, str_create(n.c_str()), *item);
            else
                return tl::nullopt;
        }
    }
    else {
        switch(tdef->get_id()) {
            case typing::ids::BOOL: {
                if(auto b = buffer::get_bool(self, idx); b) {
                    v.b_v = *b;
                    idx += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::CHAR: {
                if(auto b = buffer::get_char(self, idx); b) {
                    v.ch_v = *b;
                    idx += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::WCHAR: {
                if(auto b = buffer::get_wchar(self, idx); b) {
                    v.ch_v = *b;
                    idx += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::U8: {
                if(auto b = buffer::get_u8(self, idx); b) {
                    v.u8_v = *b;
                    idx += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::U16: {
                if(auto b = buffer::get_u16(self, idx, tdef->is_big()); b) {
                    v.u16_v = *b;
                    idx += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::U32: {
                if(auto b = buffer::get_u32(self, idx, tdef->is_big()); b) {
                    v.u32_v = *b;
                    idx += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::U64: {
                if(auto b = buffer::get_u64(self, idx, tdef->is_big()); b) {
                    v.u64_v = *b;
                    idx += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::I8: {
                if(auto b = buffer::get_i8(self, idx); b) {
                    v.i8_v = *b;
                    idx += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::I16: {
                if(auto b = buffer::get_i16(self, idx, tdef->is_big()); b) {
                    v.i16_v = *b;
                    idx += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::I32: {
                if(auto b = buffer::get_i32(self, idx, tdef->is_big()); b) {
                    v.i32_v = *b;
                    idx += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::I64: {
                if(auto b = buffer::get_i64(self, idx, tdef->is_big()); b) {
                    v.i64_v = *b;
                    idx += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::STR: {
                if(auto s = buffer::get_str(self, idx); s) {
                    v.str = str_create_n(s->c_str(), 0, s->size());
                    idx += s->size();
                }
                else
                    return tl::nullopt;
                break;
            }

            case typing::ids::WSTR: {
                if(auto s = buffer::get_wstr(self, idx); s) {
                    v.str = str_create_n(s->c_str(), 0, s->size());
                    idx += s->size();
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

tl::optional<RDValue> get_type_impl(const RDBuffer* self, usize& idx,
                                    RDType t) {
    const typing::TypeDef* td = state::get_types().get_typedef(t);
    assume(td);

    if(t.n > 0) {
        RDValue v = rdvalue_create();
        v.type = t;
        vect_reserve(&v.list, t.n);

        for(usize i = 0; i < t.n; i++) {
            auto item = buffer::get_type_impl(self, idx, td);
            if(item)
                vect_append(RDValue, &v.list, *item);
            else
                return tl::nullopt;
        }

        return v;
    }

    return buffer::get_type_impl(self, idx, td);
}

} // namespace

usize read(const RDBuffer* self, usize idx, void* dst, usize n) {
    if(!dst || (idx + n > self->length)) return 0;

    usize i = 0;
    auto* p = reinterpret_cast<u8*>(dst);

    for(; i < n; i++, p++) {
        if(auto b = buffer::get_byte(self, idx + i); b)
            *p = *b;
        else
            break;
    }

    return i;
}

tl::optional<RDValue> read_struct(const RDBuffer* self, usize idx,
                                  const RDStructField* fields) {
    if(!fields) return tl::nullopt;
    RDValue v = rdvalue_create();

    while(fields->type && fields->name) {
        typing::ParsedType pt = state::get_types().parse(fields->type);

        if(pt.n) {
            RDValue l = rdvalue_create();
            l.type = pt.to_type();
            vect_reserve(&l.list, pt.n);

            for(usize i = 0; i < pt.n; i++) {
                auto val = buffer::get_type_impl(self, idx, pt.tdef);
                if(!val) return tl::nullopt;
                vect_append(RDValue, &l.list, *val);
            }

            dict_set(RDValueField, &v.dict, str_create(fields->name), l);
        }
        else {
            auto val = buffer::get_type_impl(self, idx, pt.tdef);
            if(!val) return tl::nullopt;
            dict_set(RDValueField, &v.dict, str_create(fields->name), *val);
        }

        fields++;
    }

    return v;
}

tl::optional<std::string> get_str(const RDBuffer* self, usize idx) {
    return buffer::get_str_impl(self, idx, typing::names::CHAR);
}

tl::optional<std::string> get_str(const RDBuffer* self, usize idx, usize n) {
    return buffer::get_str_impl(self, idx, n, typing::names::CHAR);
}

tl::optional<std::string> get_wstr(const RDBuffer* self, usize idx) {
    return buffer::get_str_impl(self, idx, typing::names::WCHAR);
}

tl::optional<std::string> get_wstr(const RDBuffer* self, usize idx, usize n) {
    return buffer::get_str_impl(self, idx, n, typing::names::WCHAR);
}

tl::optional<RDValue> get_type(const RDBuffer* self, usize idx,
                               typing::FullTypeName tn) {
    typing::ParsedType pt = state::get_types().parse(tn);
    return buffer::get_type(self, idx, pt.to_type());
}

tl::optional<RDValue> get_type(const RDBuffer* self, usize idx, RDType t) {
    return buffer::get_type_impl(self, idx, t);
}

tl::optional<RDValue> get_type(const RDBuffer* self, usize idx,
                               typing::FullTypeName tn, usize& curridx) {
    typing::ParsedType pt = state::get_types().parse(tn);
    return buffer::get_type(self, idx, pt.to_type(), curridx);
}

tl::optional<RDValue> get_type(const RDBuffer* self, usize idx, RDType t,
                               usize& curridx) {
    curridx = idx;
    return buffer::get_type_impl(self, curridx, t);
}

tl::optional<u8> get_byte(const RDBuffer* self, usize idx) {
    u8 b;
    if((idx < self->length) && self->get_byte(self, idx, &b)) return b;
    return tl::nullopt;
}

tl::optional<bool> get_bool(const RDBuffer* self, usize idx) {
    auto b = buffer::get_byte(self, idx);
    if(b) return !!(*b);
    return tl::nullopt;
}

tl::optional<char> get_char(const RDBuffer* self, usize idx) {
    auto b = buffer::get_byte(self, idx);
    if(b) return static_cast<char>(*b);
    return tl::nullopt;
}

tl::optional<char> get_wchar(const RDBuffer* self, usize idx) {
    auto b = buffer::get_u16(self, idx, false);
    if(b) return static_cast<char>(*b & 0xFF);
    return tl::nullopt;
}

} // namespace redasm::buffer
