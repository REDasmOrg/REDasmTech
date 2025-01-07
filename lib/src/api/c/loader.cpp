#include "../internal/loader.h"

bool rd_registerloader(const RDLoaderPlugin* plugin) {
    return redasm::api::internal::register_loader(plugin);
}

const RDLoaderPlugin** rd_getloaderplugins(usize* n) {
    return redasm::api::internal::get_loaderplugins(n);
}

const RDLoaderPlugin* rd_getloaderplugin() {
    return redasm::api::internal::get_loaderplugin();
}

const RDLoader* rd_getloader() { return redasm::api::internal::get_loader(); }
