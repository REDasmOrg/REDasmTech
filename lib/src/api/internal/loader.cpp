#include "loader.h"
#include "../../context.h"
#include "../../state.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

const RDLoader* get_loader() {
    spdlog::trace("get_loader()");

    if(state::context)
        return state::context->loader;

    return nullptr;
}

usize get_loaders(const RDLoader** loaders) {
    spdlog::trace("get_loaders({})", fmt::ptr(loaders));

    if(loaders)
        *loaders = state::loaders.data();

    return state::loaders.size();
}

void register_loader(const RDLoader& loader) {
    spdlog::trace("register_loader('{}', '{}')", loader.id, loader.name);

    if(loader.id)
        state::loaders.push_back(loader);
    else
        spdlog::error("register_loader: invalid id");
}

} // namespace redasm::api::internal
