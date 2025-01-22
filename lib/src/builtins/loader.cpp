#include "loader.h"
#include "../api/internal/loader.h"
#include "../api/internal/memory.h"
#include "../context.h"
#include "../state.h"
#include <redasm/loader.h>

namespace redasm::builtins {

namespace {

RDLoaderPlugin binary_loader = {
    .level = REDASM_API_LEVEL,
    .id = "binary",
    .name = "Binary",
    .flags = PF_LAST,
    .accept = [](const RDLoaderPlugin*,
                 const RDLoaderRequest*) { return true; },

    .load =
        [](RDLoader*, RDBuffer*) {
            api::internal::memory_map(0, state::context->file->size());
            api::internal::memory_copy(0, 0, state::context->file->size());
            return true;
        },
};

} // namespace

void register_loaders() {
    api::internal::register_loader(&binary_loader, pm::Origin::NATIVE);
}

} // namespace redasm::builtins
