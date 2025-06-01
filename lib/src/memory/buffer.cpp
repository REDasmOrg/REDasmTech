#include "buffer.h"
#include "../context.h"
#include "../state.h"
#include "../utils/utils.h"
#include <redasm/types.h>

namespace redasm::buffer {

namespace {

template<RDPrimitiveType TID>
tl::optional<std::string> get_str_impl(const RDBuffer* self, usize idx) {
    if(idx >= self->length) return tl::nullopt;

    const RDTypeDef* def = state::get_types().get_primitive(TID);
    ct_assume(def);

    std::string s;

    for(usize curr = idx; curr < self->length; curr += def->size) {
        tl::optional<char> ch;

        if constexpr(TID == T_CHAR)
            ch = buffer::get_char(self, curr);
        else if constexpr(TID == T_WCHAR)
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

template<RDPrimitiveType TID>
tl::optional<std::string> get_str_impl(const RDBuffer* self, usize idx,
                                       usize n) {
    if(idx >= self->length) return tl::nullopt;

    const RDTypeDef* t = state::get_types().get_primitive(TID);
    const usize ENDIDX = std::min(idx + n, self->length);
    std::string s;
    s.reserve(ENDIDX - idx);

    for(usize curr = idx; curr < ENDIDX; curr += t->size) {
        tl::optional<char> ch;

        if constexpr(TID == T_CHAR)
            ch = buffer::get_char(self, curr);
        else if constexpr(TID == T_WCHAR)
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

tl::optional<RDValue> get_type_impl_2(const RDBuffer* self, usize& idx,
                                      RDType t) {
    RDValue res;
    rdvalue_init(&res);
    res.type = t;

    if(t.def->kind == TK_STRUCT) {
        const RDStructField* field;
        slice_foreach(field, &t.def->t_struct) {
            if(auto item = buffer::get_type_impl(self, idx, field->type);
               item) {
                char* k = utils::copy_str(field->name);
                auto* v = new RDValueHNode{k, *item};
                hmap_set(&res.dict, &v->hnode, k);
            }
            else
                goto fail;
        }
    }
    else if(t.def->kind == TK_PRIMITIVE) {
        usize sz = state::get_types().size_of(t);
        bool isbig = t.def->flags & TF_BIG;

        switch(t.def->t_primitive) {
            case T_BOOL: {
                if(auto b = buffer::get_bool(self, idx); b) {
                    res.b_v = *b;
                    idx += sz;
                }
                else
                    goto fail;
                break;
            }

            case T_CHAR: {
                if(auto b = buffer::get_char(self, idx); b) {
                    res.ch_v = *b;
                    idx += sz;
                }
                else
                    goto fail;
                break;
            }

            case T_WCHAR: {
                if(auto b = buffer::get_wchar(self, idx); b) {
                    res.ch_v = *b;
                    idx += sz;
                }
                else
                    goto fail;
                break;
            }

            case T_U8: {
                if(auto b = buffer::get_u8(self, idx); b) {
                    res.u8_v = *b;
                    idx += sz;
                }
                else
                    goto fail;
                break;
            }

            case T_U16:
            case T_U16BE: {
                if(auto b = buffer::get_u16(self, idx, isbig); b) {
                    res.u16_v = *b;
                    idx += sz;
                }
                else
                    goto fail;
                break;
            }

            case T_U32:
            case T_U32BE: {
                if(auto b = buffer::get_u32(self, idx, isbig); b) {
                    res.u32_v = *b;
                    idx += sz;
                }
                else
                    goto fail;
                break;
            }

            case T_U64:
            case T_U64BE: {
                if(auto b = buffer::get_u64(self, idx, isbig); b) {
                    res.u64_v = *b;
                    idx += sz;
                }
                else
                    goto fail;
                break;
            }

            case T_I8: {
                if(auto b = buffer::get_i8(self, idx); b) {
                    res.i8_v = *b;
                    idx += sz;
                }
                else
                    goto fail;
                break;
            }

            case T_I16:
            case T_I16BE: {
                if(auto b = buffer::get_i16(self, idx, isbig); b) {
                    res.i16_v = *b;
                    idx += sz;
                }
                else
                    goto fail;
                break;
            }

            case T_I32:
            case T_I32BE: {
                if(auto b = buffer::get_i32(self, idx, isbig); b) {
                    res.i32_v = *b;
                    idx += sz;
                }
                else
                    goto fail;
                break;
            }

            case T_I64:
            case T_I64BE: {
                if(auto b = buffer::get_i64(self, idx, isbig); b) {
                    res.i64_v = *b;
                    idx += sz;
                }
                else
                    goto fail;
                break;
            }

            case T_STR: {
                if(auto s = buffer::get_str(self, idx); s) {
                    res.str =
                        str_create_n(s->c_str(), s->size(), nullptr, nullptr);
                    idx += s->size();
                }
                else
                    goto fail;
                break;
            }

            case T_WSTR: {
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
    RDValue v;
    rdvalue_init(&v);

    if(t.n > 0) {
        v.type = t;
        slice_reserve(&v.list, t.n);

        auto itemtype = state::get_types().create_type(t.def);
        ct_assume(itemtype);

        for(usize i = 0; i < t.n; i++) {
            if(auto item = buffer::get_type_impl_2(self, idx, *itemtype); item)
                slice_push(&v.list, *item);
            else
                goto fail;
        }

        return v;
    }

    return buffer::get_type_impl_2(self, idx, t);

fail:
    rdvalue_destroy(&v);
    return tl::nullopt;
}

tl::optional<RDValue> read_struct_impl(const RDBuffer* self, usize& idx,
                                       const RDStructFieldDecl* fields) {
    if(!fields) return tl::nullopt;
    RDValue res;
    rdvalue_init(&res);

    while(fields->type && fields->name) {
        auto ft = state::get_types().create_type(fields->type);
        if(!ft) goto fail;

        if(ft->n) {
            RDValue l;
            rdvalue_init(&l);
            l.type = *ft;
            slice_reserve(&l.list, ft->n);

            auto itemft = state::get_types().create_type(ft->def);
            ct_assume(itemft);

            for(usize i = 0; i < ft->n; i++) {
                if(auto val = buffer::get_type_impl(self, idx, *itemft); val)
                    slice_push(&l.list, *val);
                else
                    goto fail;
            }

            auto* n = new RDValueHNode{utils::copy_str(fields->name), l};
            hmap_set(&res.dict, &n->hnode, n->key);
        }
        else if(auto v = buffer::get_type_impl(self, idx, *ft); v) {
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
                                    const RDStructFieldDecl* fields,
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
                                    const RDStructFieldDecl* fields) {
    usize cidx;
    return buffer::read_struct_n(self, idx, n, fields, cidx);
}

tl::optional<RDValue> read_struct(const RDBuffer* self, usize idx,
                                  const RDStructFieldDecl* fields,
                                  usize& curridx) {
    auto v = buffer::read_struct_impl(self, idx, fields);
    if(v) curridx = idx;
    return v;
}

tl::optional<RDValue> read_struct(const RDBuffer* self, usize idx,
                                  const RDStructFieldDecl* fields) {
    usize cidx;
    return buffer::read_struct(self, idx, fields, cidx);
}

tl::optional<std::string> get_str(const RDBuffer* self, usize idx) {
    return buffer::get_str_impl<T_CHAR>(self, idx);
}

tl::optional<std::string> get_str(const RDBuffer* self, usize idx, usize n) {
    return buffer::get_str_impl<T_CHAR>(self, idx, n);
}

tl::optional<std::string> get_wstr(const RDBuffer* self, usize idx) {
    return buffer::get_str_impl<T_WCHAR>(self, idx);
}

tl::optional<std::string> get_wstr(const RDBuffer* self, usize idx, usize n) {
    return buffer::get_str_impl<T_WCHAR>(self, idx, n);
}

tl::optional<RDValue> get_type(const RDBuffer* self, usize idx,
                               std::string_view tn) {
    auto t = state::get_types().create_type(tn);
    if(t) return buffer::get_type(self, idx, *t);
    return tl::nullopt;
}

tl::optional<RDValue> get_type(const RDBuffer* self, usize idx,
                               std::string_view tn, usize& curridx) {
    auto t = state::get_types().create_type(tn);
    if(t) return buffer::get_type(self, idx, *t, curridx);
    return tl::nullopt;
}

tl::optional<RDValue> get_type(const RDBuffer* self, usize idx, RDType t) {
    return buffer::get_type_impl(self, idx, t);
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
