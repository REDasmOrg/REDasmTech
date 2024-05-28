#include "abstractbuffer.h"
#include "../context.h"
#include "../error.h"
#include "../state.h"

namespace redasm {

tl::optional<typing::Value>
AbstractBuffer::get_type_impl(usize& pos, const typing::Type* t) const {
    typing::Value v{t->name};

    switch(t->type()) {
        case typing::types::BOOL: {
            if(auto b = this->get_bool(pos); b) {
                v.b_v = *b;
                pos += t->size;
            }
            else
                return tl::nullopt;

            break;
        }

        case typing::types::CHAR: {
            if(auto b = this->get_char(pos); b) {
                v.ch_v = *b;
                pos += t->size;
            }
            else
                return tl::nullopt;

            break;
        }

        case typing::types::WCHAR: {
            if(auto b = this->get_wchar(pos); b) {
                v.ch_v = *b;
                pos += t->size;
            }
            else
                return tl::nullopt;

            break;
        }

        case typing::types::U8: {
            if(auto b = this->get_u8(pos); b) {
                v.u8_v = *b;
                pos += t->size;
            }
            else
                return tl::nullopt;

            break;
        }

        case typing::types::U16: {
            if(auto b = this->get_u16(pos, t->is_big()); b) {
                v.u16_v = *b;
                pos += t->size;
            }
            else
                return tl::nullopt;

            break;
        }

        case typing::types::U32: {
            if(auto b = this->get_u32(pos, t->is_big()); b) {
                v.u32_v = *b;
                pos += t->size;
            }
            else
                return tl::nullopt;

            break;
        }

        case typing::types::U64: {
            if(auto b = this->get_u64(pos, t->is_big()); b) {
                v.u64_v = *b;
                pos += t->size;
            }
            else
                return tl::nullopt;

            break;
        }

        case typing::types::I8: {
            if(auto b = this->get_i8(pos); b) {
                v.i8_v = *b;
                pos += t->size;
            }
            else
                return tl::nullopt;

            break;
        }

        case typing::types::I16: {
            if(auto b = this->get_i16(pos, t->is_big()); b) {
                v.i16_v = *b;
                pos += t->size;
            }
            else
                return tl::nullopt;

            break;
        }

        case typing::types::I32: {
            if(auto b = this->get_i32(pos, t->is_big()); b) {
                v.i32_v = *b;
                pos += t->size;
            }
            else
                return tl::nullopt;

            break;
        }

        case typing::types::I64: {
            if(auto b = this->get_i64(pos, t->is_big()); b) {
                v.i64_v = *b;
                pos += t->size;
            }
            else
                return tl::nullopt;

            break;
        }

        case typing::types::STR: {
            if(auto s = this->get_string(pos); s) {
                v.str = *s;
                pos += s->size();
            }
            else
                return tl::nullopt;
            break;
        }

        case typing::types::WSTR: {
            if(auto s = this->get_wstring(pos); s) {
                v.str = *s;
                pos += s->size();
            }
            else
                return tl::nullopt;
            break;
        }

        case typing::types::STRUCT: {
            for(const auto& [t, n] : t->dict) {
                auto item = this->get_type_impl(pos, t);
                if(item)
                    v.dict[n] = *item;
                else
                    return tl::nullopt;
            }

            break;
        }

        default: unreachable;
    }

    return v;
}

tl::optional<std::string>
AbstractBuffer::get_string_impl(usize& idx, std::string_view tname) const {
    if(idx >= this->size())
        return tl::nullopt;

    const typing::Type* t = state::context->types.get_type(tname);
    assume(t);

    std::string s;

    for(usize i = idx; i < this->size(); i += t->size) {
        auto v = this->get_type(i, tname);

        if(v) {
            if(!v->ch_v) // Stop at terminator
                break;

            s += static_cast<char>(v->ch_v);
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

    const typing::Type* t = state::context->types.get_type(tname);
    assume(t);

    const usize ENDIDX = std::min(idx + n, this->size());

    for(usize i = idx; i < ENDIDX; i += t->size) {
        auto v = this->get_type(i, tname);

        if(v)
            s += static_cast<char>(v->ch_v);
        else
            break;
    }

    return s;
}

tl::optional<typing::Value>
AbstractBuffer::get_type_impl(usize& idx, std::string_view tname) const {
    auto [t, n] = typing::parse(tname);
    const typing::Type* type = state::context->types.get_type(t);

    if(n > 0) {
        typing::Value v{type->name, n};

        for(usize i = 0; i < n; i++) {
            auto item = this->get_type_impl(idx, type);
            if(item)
                v.list.push_back(*item);
            else
                return tl::nullopt;
        }

        return v;
    }

    return this->get_type_impl(idx, type);
}

tl::optional<typing::Value>
AbstractBuffer::get_type(usize idx, std::string_view tname) const {
    return this->get_type_impl(idx, tname);
}

[[nodiscard]] tl::optional<typing::Value>
AbstractBuffer::get_type(usize idx, std::string_view tname,
                         usize& lastidx) const {
    lastidx = idx;
    return this->get_type_impl(lastidx, tname);
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
