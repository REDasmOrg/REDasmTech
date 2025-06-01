#include "typing.h"
#include "../utils/utils.h"
#include "parser.h"
#include "primitive.h"
#include <fmt/core.h>

namespace redasm::typing {

BaseTypes::BaseTypes() {
    for(int i = 0; i < T_NPRIMITIVES; i++)
        this->m_registered[primitive::NAMES[i]] = &primitive::types[i];
}

BaseTypes::~BaseTypes() {
    for(auto& [_, type] : this->m_registered) {
        if(type->kind == TK_PRIMITIVE) continue;

        if(type->kind == TK_STRUCT) {
            RDStructField* field;
            slice_foreach(field, &type->t_struct) delete[] field->name;
            slice_destroy(&type->t_struct);
        }

        delete[] type->name;
        delete type;
    }
}

tl::optional<RDType> BaseTypes::create_type(const RDTypeDef* tdef,
                                            usize n) const {
    if(!tdef) return tl::nullopt;

    return RDType{
        .n = n,
        .def = tdef,
    };
}

tl::optional<RDType> BaseTypes::create_type(std::string_view tname) const {
    auto pt = typing::parse(tname);
    if(!pt) return tl::nullopt;

    RDType t = {
        .n = pt->n,
        .def = this->get_type(pt->name),
    };

    ct_exceptf_if(!t.def, "Type '%.*s' not found", tname.size(), tname.data());
    return t;
}

tl::optional<RDType> BaseTypes::int_from_bytes(usize sz, bool sign) const {
    for(const auto& [_, def] : this->m_registered) {
        if(!(def->flags & TF_INT) || (def->flags & TF_BIG)) continue;

        if(sz == def->size && sign == (def->flags & TF_SIGN))
            return RDType{.n = 0, .def = def};
    }

    return tl::nullopt;
}

RDType BaseTypes::create_primitive(RDPrimitiveType pt, usize n) const {
    const RDTypeDef* tdef = this->get_primitive(pt);
    ct_assume(tdef);
    auto t = this->create_type(tdef, n);
    ct_assume(t.has_value());
    return *t;
}

tl::optional<RDType> BaseTypes::create_type(std::string_view tname,
                                            usize n) const {
    auto pt = typing::parse(tname);
    if(!pt) return {};

    const RDTypeDef* tdef = this->get_type(pt->name);
    ct_exceptf_if(!tdef, "Type '%.*s' not found", tname.size(), tname.data());
    return this->create_type(tdef, n);
}

usize BaseTypes::size_of(std::string_view tname) const {
    if(tname.empty()) return 0;
    auto t = this->create_type(tname);
    ct_assume(t);
    return this->size_of(t.value());
}

usize BaseTypes::size_of(RDType t) const {
    if(!t.def) return 0;
    if(t.n > 0) return t.def->size * t.n;
    return t.def->size;
}

const RDTypeDef* BaseTypes::get_primitive(RDPrimitiveType pt) const {
    ct_exceptf_if(pt >= primitive::types.size(),
                  "Primitive type #%d is not valid", pt);
    return &primitive::types[pt];
}

const RDTypeDef* BaseTypes::get_type(std::string_view name) const {
    auto it = m_registered.find(name);
    ct_exceptf_if(it == m_registered.end(), "Type '%.*s' not found",
                  name.size(), name.data());
    return it->second;
}

std::string BaseTypes::as_array(RDPrimitiveType pt, usize n) const {
    const RDTypeDef* td = this->get_primitive(pt);
    ct_assume(td);
    return this->as_array(td->name, n);
}

std::string BaseTypes::as_array(std::string_view name, usize n) const {
    return fmt::format("{}[{}]", name, n);
}

std::string BaseTypes::to_string(RDType t) const {
    if(t.n > 0) return fmt::format("{}[{}]", t.def->name, t.n);
    return std::string{t.def->name};
}

void BaseTypes::destroy_type(RDTypeDef* t) {
    if(!t) return;

    // 1. Remove type from registered
    auto it = m_registered.begin();
    while(it != m_registered.end()) {
        if(it->second == t) break;
        it++;
    }
    if(it != m_registered.end()) m_registered.erase(it);

    // 2. Deallocate type
    switch(t->kind) {
        case TK_PRIMITIVE: return;

        case TK_STRUCT: {
            delete[] t->name;
            const RDStructField* it;
            slice_foreach(it, &t->t_struct) {
                this->destroy_type(const_cast<RDTypeDef*>(it->type.def));
            }
            slice_destroy(&t->t_struct);
            return;
        }

        case TK_FUNCTION: {
            // TODO(davide): Function type deallocation
            return;
        }

        default: break;
    }

    ct_unreachable;
}

RDTypeDef* Types::create_struct(const std::string& name,
                                const RDStructFieldDecl* fields) {
    ct_except_if(name.empty(), "Struct name is empty");

    if(!fields || !fields->name || !fields->type)
        ct_exceptf("Struct '%s' cannot be empty", name.c_str());

    if(this->m_registered.contains(name))
        ct_exceptf("Type '%s' already registered", name.c_str());

    auto* t = new RDTypeDef{};
    t->kind = TK_STRUCT;
    t->name = utils::copy_str(name);
    t->size = 0;
    slice_init(&t->t_struct, nullptr, nullptr);

    while(fields && fields->type) {
        ct_except_if(!fields->name,
                     "An empty struct field-name has been found");

        auto pt = typing::parse(fields->type);
        ct_exceptf_if(!pt, "Invalid type %s", fields->type);

        auto ft = this->create_type(fields->type, pt->n);
        ct_exceptf_if(!ft, "Unknown type %s", fields->type);

        slice_push(&t->t_struct, {
                                     .type = *ft,
                                     .name = utils::copy_str(fields->name),
                                 });

        t->size += this->size_of(*ft);
        fields++;
    }

    m_registered[name] = t;
    return t;
}

} // namespace redasm::typing
