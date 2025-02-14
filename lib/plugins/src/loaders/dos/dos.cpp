#include "dos.h"

namespace dos {

namespace {

bool accept(RDLoader*, const RDLoaderRequest* req) { return false; }

bool load(RDLoader*, RDBuffer* file) { return false; }

} // namespace

RDLoaderPlugin loader = {
    .level = REDASM_API_LEVEL,
    .id = "dos",
    .name = "DOS Executable",
    .accept = dos::accept,
    .load = dos::load,
};

} // namespace dos
