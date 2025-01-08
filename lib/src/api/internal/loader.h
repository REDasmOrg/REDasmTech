#pragma once

#include "../../plugins/origin.h"
#include <redasm/loader.h>

namespace redasm::api::internal {

bool register_loader(const RDLoaderPlugin* plugin, pm::Origin o);
const RDLoaderPlugin** get_loaderplugins(usize* n);
const RDLoaderPlugin* get_loaderplugin();
RDLoader* get_loader();

} // namespace redasm::api::internal
