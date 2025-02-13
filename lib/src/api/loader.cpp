#include "../context.h"
#include "../plugins/pluginmanager.h"
#include "../state.h"
#include <spdlog/spdlog.h>

bool rd_registerloader(const RDLoaderPlugin* plugin) {
    spdlog::trace("rd_registerloader({})", fmt::ptr(plugin));
    return redasm::pm::register_loader(plugin, redasm::pm::NATIVE);
}

bool rd_registerloader_ex(const RDLoaderPlugin* plugin, const char* origin) {
    spdlog::trace("rd_registerloader_ex({}, '{}')", fmt::ptr(plugin), origin);
    return redasm::pm::register_loader(plugin, origin);
}

Vect(const RDLoaderPlugin*) rd_getloaderplugins() {
    spdlog::trace("rd_getloaderplugins()");
    return redasm::pm::loaders;
}

const RDLoaderPlugin* rd_getloaderplugin() {
    spdlog::trace("rd_getloaderplugin()");
    if(redasm::state::context) return redasm::state::context->loaderplugin;
    return nullptr;
}

RDLoader* rd_getloader() {
    spdlog::trace("rd_getloader()");
    if(redasm::state::context) return redasm::state::context->loader;
    return nullptr;
}
