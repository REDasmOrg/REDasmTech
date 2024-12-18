#include "../internal/loader.h"

const RDLoader* rd_getloader(void) {
    return redasm::api::internal::get_loader();
}

usize rd_getloaders(const RDLoader** loaders) {
    return redasm::api::internal::get_loaders(loaders);
}

void rd_registerloader(const RDLoader* ldr) {
    if(ldr)
        redasm::api::internal::register_loader(*ldr);
}
