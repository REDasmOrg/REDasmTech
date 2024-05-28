#include "typing.h"
#include "../../context.h"
#include "../../state.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

usize size_of(std::string_view tname) {
    spdlog::trace("size_of('{}')", tname);
    return state::context->types.size_of(tname);
}

std::string create_struct(const std::string& name,
                          const typing::StructBody& fields) {
    spdlog::trace("create_struct('{}', {} fields)", name, fields.size());
    typing::Type t = state::context->types.create_struct(fields);
    state::context->types.declare(name, t);
    return name;
}

} // namespace redasm::api::internal
