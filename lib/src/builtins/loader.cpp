#include "loader.h"
#include "../api/internal/loader.h"
#include "../api/internal/memory.h"
#include "../context.h"
#include "../state.h"

namespace redasm::builtins {

void register_loaders() {
    RDLoader binary{};
    binary.name = "Binary";

    binary.init = [](RDLoader*) {
        api::internal::memory_map(0, state::context->file->size());
        api::internal::memory_copy(0, 0, state::context->file->size());
        return true;
    };

    api::internal::register_loader(binary);
}

} // namespace redasm::builtins
