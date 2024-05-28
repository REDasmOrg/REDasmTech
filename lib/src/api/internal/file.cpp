#include "file.h"
#include "../../context.h"
#include "../../state.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

usize file_size() {
    spdlog::trace("file_size()");
    return state::context->file->size();
}

tl::optional<typing::Value> file_map_type(RDOffset offset,
                                          std::string_view tname) {
    spdlog::trace("file_map_type({:x}, '{}')", offset, tname);
    if(!state::context->set_type(offset, tname))
        return tl::nullopt;

    usize sz = state::context->types.size_of(tname);
    state::context->memory_copy(offset, offset, offset + sz);
    return state::context->file->get_type(offset, tname);
}

tl::optional<typing::Value> file_map_type_as(RDOffset offset,
                                             std::string_view tname,
                                             const std::string& dbname) {
    spdlog::trace("file_map_type_as({:x}, '{}', '{}')", offset, tname, dbname);

    if(!state::context->set_type(offset, tname, dbname))
        return tl::nullopt;

    usize sz = state::context->types.size_of(tname);
    state::context->memory_copy(offset, offset, offset + sz);
    return state::context->file->get_type(offset, tname);
}

} // namespace redasm::api::internal
