#include "abstractbuffer.h"
#include "../error.h"
#include "../state.h"
#include "../typing/base.h"

namespace redasm {

usize AbstractBuffer::read(usize idx, void* dst, usize n) const {
    if(!dst || (idx + n > this->size())) return 0;

    usize i = 0;
    auto* p = reinterpret_cast<u8*>(dst);

    for(; i < n; i++, p++) {
        auto b = this->get_byte(idx++);
        if(b)
            *p = *b;
        else
            break;
    }

    return i;
}

tl::optional<RDValue>
AbstractBuffer::get_type_impl(usize& pos, const typing::TypeDef* tdef) const {
    RDValue v = rdvalue_create();
    v.type = tdef->to_type();

    if(tdef->is_struct()) {
        for(const auto& [t, n] : tdef->dict) {
            auto item = this->get_type_impl(pos, t);
            if(item)
                dict_set(RDValueField, &v.dict, str_create(n.c_str()), *item);
            else
                return tl::nullopt;
        }
    }
    else {
        switch(tdef->get_id()) {
            case typing::ids::BOOL: {
                if(auto b = this->get_bool(pos); b) {
                    v.b_v = *b;
                    pos += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::CHAR: {
                if(auto b = this->get_char(pos); b) {
                    v.ch_v = *b;
                    pos += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::WCHAR: {
                if(auto b = this->get_wchar(pos); b) {
                    v.ch_v = *b;
                    pos += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::U8: {
                if(auto b = this->get_u8(pos); b) {
                    v.u8_v = *b;
                    pos += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::U16: {
                if(auto b = this->get_u16(pos, tdef->is_big()); b) {
                    v.u16_v = *b;
                    pos += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::U32: {
                if(auto b = this->get_u32(pos, tdef->is_big()); b) {
                    v.u32_v = *b;
                    pos += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::U64: {
                if(auto b = this->get_u64(pos, tdef->is_big()); b) {
                    v.u64_v = *b;
                    pos += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::I8: {
                if(auto b = this->get_i8(pos); b) {
                    v.i8_v = *b;
                    pos += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::I16: {
                if(auto b = this->get_i16(pos, tdef->is_big()); b) {
                    v.i16_v = *b;
                    pos += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::I32: {
                if(auto b = this->get_i32(pos, tdef->is_big()); b) {
                    v.i32_v = *b;
                    pos += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::I64: {
                if(auto b = this->get_i64(pos, tdef->is_big()); b) {
                    v.i64_v = *b;
                    pos += tdef->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::STR: {
                if(auto s = this->get_str(pos); s) {
                    v.str = str_create_n(s->c_str(), 0, s->size());
                    pos += s->size();
                }
                else
                    return tl::nullopt;
                break;
            }

            case typing::ids::WSTR: {
                if(auto s = this->get_wstr(pos); s) {
                    v.str = str_create_n(s->c_str(), 0, s->size());
                    pos += s->size();
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

tl::optional<std::string>
AbstractBuffer::get_str_impl(usize& idx, typing::TypeName tname) const {
    if(idx >= this->size()) return tl::nullopt;

    const typing::TypeDef* t = state::get_types().get_typedef(tname);
    assume(t);

    std::string s;

    for(usize i = idx; i < this->size(); i += t->size) {
        auto v = this->get_type(i, tname);

        if(v) {
            if(!v->ch_v) // Stop at terminator
                break;

            s += v->ch_v;
        }
        else
            break;
    }

    return s;
}

tl::optional<std::string>
AbstractBuffer::get_str_impl(usize& idx, usize n,
                             std::string_view tname) const {
    std::string s;
    s.reserve(n);

    const typing::TypeDef* t = state::get_types().get_typedef(tname);
    assume(t);

    const usize ENDIDX = std::min(idx + n, this->size());

    for(usize i = idx; i < ENDIDX; i += t->size) {
        auto v = this->get_type(i, tname);

        if(v)
            s += v->ch_v;
        else
            break;
    }

    return s;
}

tl::optional<RDValue> AbstractBuffer::get_type_impl(usize& idx,
                                                    RDType t) const {
    const typing::TypeDef* td = nullptr;
    if(state::context)
        td = state::get_types().get_typedef(t);
    else
        td = state::BASETYPES.get_typedef(t);
    assume(td);

    if(t.n > 0) {
        RDValue v = rdvalue_create();
        v.type = t;
        vect_reserve(&v.list, t.n);

        for(usize i = 0; i < t.n; i++) {
            auto item = this->get_type_impl(idx, td);
            if(item)
                vect_append(RDValue, &v.list, *item);
            else
                return tl::nullopt;
        }

        return v;
    }

    return this->get_type_impl(idx, td);
}

tl::optional<RDValue> AbstractBuffer::get_type(usize idx,
                                               typing::FullTypeName tn) const {
    typing::ParsedType pt = state::get_types().parse(tn);
    return this->get_type(idx, pt.to_type());
}

[[nodiscard]]
tl::optional<RDValue>
AbstractBuffer::read_struct(usize idx, const RDStructField* fields) const {
    if(!fields) return tl::nullopt;

    RDValue v = rdvalue_create();

    while(fields->type && fields->name) {
        typing::ParsedType pt = state::get_types().parse(fields->type);

        if(pt.n) {
            RDValue l = rdvalue_create();
            l.type = pt.to_type();
            vect_reserve(&l.list, pt.n);

            for(usize i = 0; i < pt.n; i++) {
                auto val = this->get_type_impl(idx, pt.tdef);
                if(!val) return tl::nullopt;
                vect_append(RDValue, &l.list, *val);
            }

            dict_set(RDValueField, &v.dict, str_create(fields->name), l);
        }
        else {
            auto val = this->get_type_impl(idx, pt.tdef);
            if(!val) return tl::nullopt;
            dict_set(RDValueField, &v.dict, str_create(fields->name), *val);
        }

        fields++;
    }

    return v;
}

tl::optional<RDValue> AbstractBuffer::get_type(usize idx, RDType t) const {
    return this->get_type_impl(idx, t);
}

[[nodiscard]] tl::optional<RDValue>
AbstractBuffer::get_type(usize idx, typing::FullTypeName tn,
                         usize& lastidx) const {
    typing::ParsedType pt = state::get_types().parse(tn);
    return this->get_type(idx, pt.to_type(), lastidx);
}

[[nodiscard]] tl::optional<RDValue>
AbstractBuffer::get_type(usize idx, RDType t, usize& lastidx) const {
    lastidx = idx;
    return this->get_type_impl(lastidx, t);
}

tl::optional<std::string> AbstractBuffer::get_str(usize idx) const {
    return this->get_str_impl(idx, typing::names::CHAR);
}

tl::optional<std::string> AbstractBuffer::get_str(usize idx, usize n) const {
    return this->get_str_impl(idx, n, typing::names::CHAR);
}

tl::optional<std::string> AbstractBuffer::get_wstr(usize idx) const {
    return this->get_str_impl(idx, typing::names::WCHAR);
}

tl::optional<std::string> AbstractBuffer::get_wstr(usize idx, usize n) const {

    return this->get_str_impl(idx, n, typing::names::WCHAR);
}

} // namespace redasm
