#include "file.h"
#include "../../context.h"
#include "../../state.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

usize file_size() {
    spdlog::trace("file_size()");
    return state::context->program.file->size();
}

tl::optional<typing::Value> file_map_type(RDOffset offset,
                                          typing::FullTypeName tname) {
    spdlog::trace("file_map_type({:x}, '{}')", offset, tname);
    if(!state::context->set_type(offset, tname, 0))
        return tl::nullopt;

    usize sz = state::context->types.size_of(tname);
    state::context->memory_copy(offset, offset, offset + sz);
    return state::context->program.file->get_type(offset, tname);
}

tl::optional<typing::Value> file_map_type_as(RDOffset offset,
                                             typing::FullTypeName tname,
                                             const std::string& dbname) {
    spdlog::trace("file_map_type_as({:x}, '{}', '{}')", offset, tname, dbname);

    usize sz = state::context->types.size_of(tname);
    state::context->memory_copy(offset, offset, offset + sz);
    return state::context->program.file->get_type(offset, tname);
}

} // namespace redasm::api::internal
