#include "abstractbuffer.h"
#include "../context.h"
#include "../error.h"
#include "../state.h"

namespace redasm {

tl::optional<typing::Value>
AbstractBuffer::get_type_impl(usize& pos, const typing::TypeDef* t) const {
    typing::Value v;
    v.type = t->to_type();

    if(t->is_struct()) {
        for(const auto& [t, n] : t->dict) {
            auto item = this->get_type_impl(pos, t);
            if(item)
                v.dict[n] = *item;
            else
                return tl::nullopt;
        }
    }
    else {
        switch(t->get_id()) {
            case typing::ids::BOOL: {
                if(auto b = this->get_bool(pos); b) {
                    v.b_v = *b;
                    pos += t->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::CHAR: {
                if(auto b = this->get_char(pos); b) {
                    v.ch_v = *b;
                    pos += t->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::WCHAR: {
                if(auto b = this->get_wchar(pos); b) {
                    v.ch_v = *b;
                    pos += t->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::U8: {
                if(auto b = this->get_u8(pos); b) {
                    v.u8_v = *b;
                    pos += t->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::U16: {
                if(auto b = this->get_u16(pos, t->isbig); b) {
                    v.u16_v = *b;
                    pos += t->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::U32: {
                if(auto b = this->get_u32(pos, t->isbig); b) {
                    v.u32_v = *b;
                    pos += t->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::U64: {
                if(auto b = this->get_u64(pos, t->isbig); b) {
                    v.u64_v = *b;
                    pos += t->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::I8: {
                if(auto b = this->get_i8(pos); b) {
                    v.i8_v = *b;
                    pos += t->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::I16: {
                if(auto b = this->get_i16(pos, t->isbig); b) {
                    v.i16_v = *b;
                    pos += t->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::I32: {
                if(auto b = this->get_i32(pos, t->isbig); b) {
                    v.i32_v = *b;
                    pos += t->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::I64: {
                if(auto b = this->get_i64(pos, t->isbig); b) {
                    v.i64_v = *b;
                    pos += t->size;
                }
                else
                    return tl::nullopt;

                break;
            }

            case typing::ids::STR: {
                if(auto s = this->get_string(pos); s) {
                    v.str = *s;
                    pos += s->size();
                }
                else
                    return tl::nullopt;
                break;
            }

            case typing::ids::WSTR: {
                if(auto s = this->get_wstring(pos); s) {
                    v.str = *s;
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
AbstractBuffer::get_string_impl(usize& idx, typing::TypeName tname) const {
    if(idx >= this->size())
        return tl::nullopt;

    const typing::TypeDef* t = state::context->types.get_typedef(tname);
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
AbstractBuffer::get_string_impl(usize& idx, usize n,
                                std::string_view tname) const {
    if(idx >= this->size())
        return tl::nullopt;

    std::string s;
    s.reserve(n);

    const typing::TypeDef* t = state::context->types.get_typedef(tname);
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

tl::optional<typing::Value> AbstractBuffer::get_type_impl(usize& idx,
                                                          RDType t) const {
    const typing::TypeDef* td = state::context->types.get_typedef(t);
    assume(td);

    if(t.n > 0) {
        typing::Value v;
        v.type = t;

        for(usize i = 0; i < t.n; i++) {
            auto item = this->get_type_impl(idx, td);
            if(item)
                v.list.push_back(*item);
            else
                return tl::nullopt;
        }

        return v;
    }

    return this->get_type_impl(idx, td);
}

tl::optional<typing::Value>
AbstractBuffer::get_type(usize idx, typing::FullTypeName tn) const {
    typing::ParsedType pt = state::context->types.parse(tn);
    return this->get_type(idx, pt.to_type());
}

tl::optional<typing::Value> AbstractBuffer::get_type(usize idx,
                                                     RDType t) const {
    return this->get_type_impl(idx, t);
}

[[nodiscard]] tl::optional<typing::Value>
AbstractBuffer::get_type(usize idx, typing::FullTypeName tn,
                         usize& lastidx) const {
    typing::ParsedType pt = state::context->types.parse(tn);
    return this->get_type(idx, pt.to_type(), lastidx);
}

[[nodiscard]] tl::optional<typing::Value>
AbstractBuffer::get_type(usize idx, RDType t, usize& lastidx) const {
    lastidx = idx;
    return this->get_type_impl(lastidx, t);
}

tl::optional<std::string> AbstractBuffer::get_string(usize idx) const {
    return this->get_string_impl(idx, typing::names::CHAR);
}

tl::optional<std::string> AbstractBuffer::get_string(usize idx, usize n) const {
    return this->get_string_impl(idx, n, typing::names::CHAR);
}

tl::optional<std::string> AbstractBuffer::get_wstring(usize idx) const {
    return this->get_string_impl(idx, typing::names::WCHAR);
}

tl::optional<std::string> AbstractBuffer::get_wstring(usize idx,
                                                      usize n) const {

    return this->get_string_impl(idx, n, typing::names::WCHAR);
}

} // namespace redasm
