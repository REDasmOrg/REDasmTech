#include "loader.h"
#include "../api/internal/loader.h"
#include "../api/internal/memory.h"
#include "../context.h"
#include "../state.h"

namespace redasm::builtins {

namespace {

RDLoaderPlugin binary_loader = {
    .id = "binary",
    .name = "Binary",

    .load =
        [](RDLoader*) {
            api::internal::memory_map(0, state::context->file->size());
            api::internal::memory_copy(0, 0, state::context->file->size());
            return true;
        },
};

} // namespace

void register_loaders() { api::internal::register_loader(&binary_loader); }

} // namespace redasm::builtins
