#include "../internal/loader.h"

usize rd_getloaders(const RDLoader** loaders) {
    return redasm::api::internal::get_loaders(loaders);
}
