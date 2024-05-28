#include "loader.h"
#include "../../state.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

void register_loader(const RDLoader& loader) {
    spdlog::trace("register_loader('{}')", loader.name);

    if(loader.name)
        state::loaders.push_front(loader);
    else
        spdlog::error("register_loader: invalid name");
}

} // namespace redasm::api::internal
