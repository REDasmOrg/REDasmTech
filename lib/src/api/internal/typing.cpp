#include "typing.h"
#include "../../context.h"
#include "../../state.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

usize size_of(std::string_view tname) {
    spdlog::trace("size_of('{}')", tname);
    return state::get_types().size_of(tname);
}

usize size_of(const RDType* t) {
    spdlog::trace("size_of('{}')", fmt::ptr(t));
    if(t) return state::get_types().size_of(*t);
    return 0;
}

typing::Value* create_value() {
    if(state::context) return &state::context->types.valuespool.emplace_front();
    return nullptr;
}

void destroy_value(typing::Value* v) {
    state::context->types.valuespool.remove_if(
        [v](const typing::Value& x) { return v == &x; });
}

std::string type_name(RDType t) { return state::get_types().to_string(t); }

bool create_type(std::string_view tname, RDType* t) {
    spdlog::trace("create_type('{}', {})", tname, fmt::ptr(t));

    if(t) {
        *t = state::get_types().parse(tname).to_type();
        return true;
    }

    return false;
}

bool create_type_n(std::string_view tname, usize n, RDType* t) {
    spdlog::trace("create_type_n('{}', {}, {})", tname, n, fmt::ptr(t));

    if(t) {
        *t = state::get_types().parse(tname).to_type();
        t->n = n;
        return true;
    }

    return false;
}

bool int_from_bytes(usize b, bool sign, RDType* t) {
    spdlog::trace("int_from_bytes({}, {}, {})", b, sign, fmt::ptr(t));

    if(t) {
        auto inttype = state::get_types().int_from_bytes(b, sign);
        if(inttype) *t = *inttype;
        return inttype.has_value();
    }

    return false;
}

std::string create_struct(const std::string& name,
                          const typing::Struct& fields) {
    spdlog::trace("create_struct('{}', <{} fields>)", name, fields.size());

    if(state::context) {
        assume(state::context->types.declare(name, fields));
        return name;
    }

    return {};
}

} // namespace redasm::api::internal
