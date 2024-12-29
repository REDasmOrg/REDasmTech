#include "typing.h"
#include "../../context.h"
#include "../../state.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

usize size_of(std::string_view tname) {
    spdlog::trace("size_of('{}')", tname);

    if(state::context)
        return state::context->types.size_of(tname);
    return 0;
}

typing::Value* create_value() {
    if(state::context)
        return &state::context->types.valuespool.emplace_front();

    return nullptr;
}

std::string type_name(RDType t) {
    if(state::context)
        return state::context->types.to_string(t);

    return {};
}

bool create_type(std::string_view tname, RDType* t) {
    spdlog::trace("create_type('{}', {})", tname, fmt::ptr(t));

    if(t && state::context) {
        *t = state::context->types.parse(tname).to_type();
        return true;
    }

    return false;
}

std::string create_struct(const std::string& name,
                          const typing::StructBody& fields) {
    spdlog::trace("create_struct('{}', <{} fields>)", name, fields.size());
    if(state::context) {
        assume(state::context->types.declare(name, fields));
        return name;
    }

    return {};
}

} // namespace redasm::api::internal
