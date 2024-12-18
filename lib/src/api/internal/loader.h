#pragma once

#include <redasm/loader.h>

namespace redasm::api::internal {

const RDLoader* get_loader();
usize get_loaders(const RDLoader** loaders);
void register_loader(const RDLoader& loader);

} // namespace redasm::api::internal
