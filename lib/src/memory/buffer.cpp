#include "buffer.h"
#include "../state.h"
#include "../utils/utils.h"
#include <redasm/types.h>

namespace redasm::buffer {

namespace {

template<TypeId TID>
tl::optional<std::string> get_str_impl(const RDBuffer* self, usize idx) {
    if(idx >= self->length) return tl::nullopt;

    const typing::TypeDef* t = state::get_types().get_typedef(TID);
    ct_assume(t);

    std::string s;

    for(usize curr = idx; curr < self->length; curr += t->size) {
        tl::optional<char> ch;

        if constexpr(TID == typing::ids::CHAR)
            ch = buffer::get_char(self, curr);
        else if constexpr(TID == typing::ids::WCHAR)
            ch = buffer::get_wchar(self, curr);
        else
            ct_exceptf("Unsupported char type #%08x", TID);

        if(ch && *ch)
            s += *ch;
        else // Stop at terminator
            break;
    }

    return s;
}

template<TypeId TID>
tl::optional<std::string> get_str_impl(const RDBuffer* self, usize idx,
                                       usize n) {
    if(idx >= self->length) return tl::nullopt;

    const typing::TypeDef* t = state::get_types().get_typedef(TID);
    ct_assume(t);

    const usize ENDIDX = std::min(idx + n, self->length);
    std::string s;
    s.reserve(ENDIDX - idx);

    for(usize curr = idx; curr < ENDIDX; curr += t->size) {
        tl::optional<char> ch;

        if constexpr(TID == typing::ids::CHAR)
            ch = buffer::get_char(self, curr);
        else if constexpr(TID == typing::ids::WCHAR)
            ch = buffer::get_wchar(self, curr);
        else
            ct_exceptf("Unsupported char type #%08x", TID);

        if(ch)
            s += *ch;
        else // Stop at terminator
            break;
    }

    return s;
}

tl::optional<RDValue> get_type_impl(const RDBuffer* self, usize& idx, RDType t);

tl::optional<RDValue> get_type_impl(const RDBuffer* self, usize& idx,
                                    const typing::TypeDef* tdef) {
    RDValue res;
    rdvalue_init(&res);
    res.type = tdef->to_type();

    if(tdef->is_struct()) {
        for(const auto& [t, n] : tdef->dict) {
            if(auto item = buffer::get_type_impl(self, idx, t); item) {
                char* k = utils::copy_str(n);
                auto* v = new RDValueHNode{k, *item};
                hmap_set(&res.dict, &v->hnode, k);
            }
            else
                goto fail;
        }
    }
    else {
        switch(tdef->get_id()) {
            case typing::ids::BOOL: {
                if(auto b = buffer::get_bool(self, idx); b) {
                    res.b_v = *b;
                    idx += tdef->size;
                }
                else
                    goto fail;
                break;
            }

            case typing::ids::CHAR: {
                if(auto b = buffer::get_char(self, idx); b) {
                    res.ch_v = *b;
                    idx += tdef->size;
                }
                else
                    goto fail;
                break;
            }

            case typing::ids::WCHAR: {
                if(auto b = buffer::get_wchar(self, idx); b) {
                    res.ch_v = *b;
                    idx += tdef->size;
                }
                else
                    goto fail;
                break;
            }

            case typing::ids::U8: {
                if(auto b = buffer::get_u8(self, idx); b) {
                    res.u8_v = *b;
                    idx += tdef->size;
                }
                else
                    goto fail;
                break;
            }

            case typing::ids::U16: {
                if(auto b = buffer::get_u16(self, idx, tdef->is_big()); b) {
                    res.u16_v = *b;
                    idx += tdef->size;
                }
                else
                    goto fail;
                break;
            }

            case typing::ids::U32: {
                if(auto b = buffer::get_u32(self, idx, tdef->is_big()); b) {
                    res.u32_v = *b;
                    idx += tdef->size;
                }
                else
                    goto fail;
                break;
            }

            case typing::ids::U64: {
                if(auto b = buffer::get_u64(self, idx, tdef->is_big()); b) {
                    res.u64_v = *b;
                    idx += tdef->size;
                }
                else
                    goto fail;
                break;
            }

            case typing::ids::I8: {
                if(auto b = buffer::get_i8(self, idx); b) {
                    res.i8_v = *b;
                    idx += tdef->size;
                }
                else
                    goto fail;
                break;
            }

            case typing::ids::I16: {
                if(auto b = buffer::get_i16(self, idx, tdef->is_big()); b) {
                    res.i16_v = *b;
                    idx += tdef->size;
                }
                else
                    goto fail;
                break;
            }

            case typing::ids::I32: {
                if(auto b = buffer::get_i32(self, idx, tdef->is_big()); b) {
                    res.i32_v = *b;
                    idx += tdef->size;
                }
                else
                    goto fail;
                break;
            }

            case typing::ids::I64: {
                if(auto b = buffer::get_i64(self, idx, tdef->is_big()); b) {
                    res.i64_v = *b;
                    idx += tdef->size;
                }
                else
                    goto fail;
                break;
            }

            case typing::ids::STR: {
                if(auto s = buffer::get_str(self, idx); s) {
                    res.str =
                        str_create_n(s->c_str(), s->size(), nullptr, nullptr);
                    idx += s->size();
                }
                else
                    goto fail;
                break;
            }

            case typing::ids::WSTR: {
                if(auto s = buffer::get_wstr(self, idx); s) {
                    res.str =
                        str_create_n(s->c_str(), s->size(), nullptr, nullptr);
                    idx += s->size();
                }
                else
                    goto fail;
                break;
            }

            default: ct_unreachable;
        }
    }

    return res;

fail:
    rdvalue_destroy(&res);
    return tl::nullopt;
}

tl::optional<RDValue> get_type_impl(const RDBuffer* self, usize& idx,
                                    RDType t) {
    const typing::TypeDef* td = state::get_types().get_typedef(t);
    ct_assume(td);

    RDValue v;
    rdvalue_init(&v);

    if(t.n > 0) {
        v.type = t;
        slice_reserve(&v.list, t.n);

        for(usize i = 0; i < t.n; i++) {
            if(auto item = buffer::get_type_impl(self, idx, td); item)
                v.list.data[i] = *item;
            else
                goto fail;
        }

        return v;
    }

    return buffer::get_type_impl(self, idx, td);

fail:
    rdvalue_destroy(&v);
    return tl::nullopt;
}

tl::optional<RDValue> read_struct_impl(const RDBuffer* self, usize& idx,
                                       const RDStructField* fields) {
    if(!fields) return tl::nullopt;
    RDValue res;
    rdvalue_init(&res);

    while(fields->type && fields->name) {
        typing::ParsedType pt = state::get_types().parse(fields->type);

        if(pt.n) {
            RDValue l;
            rdvalue_init(&l);
            l.type = pt.to_type();
            slice_reserve(&l.list, pt.n);

            for(usize i = 0; i < pt.n; i++) {
                if(auto val = buffer::get_type_impl(self, idx, pt.tdef); val)
                    l.list.data[i] = *val;
                else
                    goto fail;
            }

            auto* n = new RDValueHNode{utils::copy_str(fields->name), l};
            hmap_set(&res.dict, &n->hnode, n->key);
        }
        else if(auto v = buffer::get_type_impl(self, idx, pt.tdef); v) {
            auto* n = new RDValueHNode{utils::copy_str(fields->name), *v};
            hmap_set(&res.dict, &n->hnode, n->key);
        }
        else
            goto fail;

        fields++;
    }

    return res;

fail:
    rdvalue_destroy(&res);
    return tl::nullopt;
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

tl::optional<RDValue> read_struct_n(const RDBuffer* self, usize idx, usize n,
                                    const RDStructField* fields,
                                    usize& curridx) {
    if(!n) return buffer::read_struct_impl(self, idx, fields);

    RDValue res;
    rdvalue_init(&res);
    slice_reserve(&res.list, n);

    for(usize i = 0; i < n; i++) {
        auto item = buffer::read_struct_impl(self, idx, fields);
        if(!item) goto fail;
    }

    curridx = idx;
    return res;

fail:
    rdvalue_destroy(&res);
    return tl::nullopt;
}

tl::optional<RDValue> read_struct_n(const RDBuffer* self, usize idx, usize n,
                                    const RDStructField* fields) {
    usize cidx;
    return buffer::read_struct_n(self, idx, n, fields, cidx);
}

tl::optional<RDValue> read_struct(const RDBuffer* self, usize idx,
                                  const RDStructField* fields, usize& curridx) {
    auto v = buffer::read_struct_impl(self, idx, fields);
    if(v) curridx = idx;
    return v;
}

tl::optional<RDValue> read_struct(const RDBuffer* self, usize idx,
                                  const RDStructField* fields) {
    usize cidx;
    return buffer::read_struct(self, idx, fields, cidx);
}

tl::optional<std::string> get_str(const RDBuffer* self, usize idx) {
    return buffer::get_str_impl<typing::ids::CHAR>(self, idx);
}

tl::optional<std::string> get_str(const RDBuffer* self, usize idx, usize n) {
    return buffer::get_str_impl<typing::ids::CHAR>(self, idx, n);
}

tl::optional<std::string> get_wstr(const RDBuffer* self, usize idx) {
    return buffer::get_str_impl<typing ::ids::WCHAR>(self, idx);
}

tl::optional<std::string> get_wstr(const RDBuffer* self, usize idx, usize n) {
    return buffer::get_str_impl<typing ::ids::WCHAR>(self, idx, n);
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
    if(self->get_byte(self, idx, &b)) return b;
    return tl::nullopt;
}

tl::optional<RDMByte> get_mbyte(const RDBuffer* self, usize idx) {
    if(idx < self->length) return self->m_data[idx];
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
