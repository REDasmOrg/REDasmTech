#include "../internal/loader.h"
#include "../../plugins/origin.h"

bool rd_registerloader(const RDLoaderPlugin* plugin) {
    return redasm::api::internal::register_loader(plugin,
                                                  redasm::pm::Origin::NATIVE);
}

const RDLoaderPlugin** rd_getloaderplugins(usize* n) {
    return redasm::api::internal::get_loaderplugins(n);
}

const RDLoaderPlugin* rd_getloaderplugin() {
    return redasm::api::internal::get_loaderplugin();
}

RDLoader* rd_getloader() { return redasm::api::internal::get_loader(); }
