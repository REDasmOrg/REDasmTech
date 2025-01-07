#include "loader.h"
#include "../../context.h"
#include "../../plugins/pluginmanager.h"
#include "../../state.h"
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

bool register_loader(const RDLoaderPlugin* plugin) {
    spdlog::trace("register_loader({})", fmt::ptr(plugin));
    return pm::register_loader(plugin);
}

const RDLoaderPlugin** get_loaderplugins(usize* n) {
    spdlog::trace("get_loaderplugins({})", fmt::ptr(n));
    return pm::get_loaders(n);
}

const RDLoaderPlugin* get_loaderplugin() {
    spdlog::trace("get_loaderplugin()");
    if(state::context) return state::context->loaderplugin;
    return nullptr;
}

RDLoader* get_loader() {
    spdlog::trace("get_loader()");
    if(state::context) return state::context->loader;
    return nullptr;
}

} // namespace redasm::api::internal
