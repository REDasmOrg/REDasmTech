#include "dos.h"

namespace dos {

namespace {

bool parse(RDLoader*, const RDLoaderRequest* req) { return false; }

bool load(RDLoader*, RDBuffer* file) { return false; }

} // namespace

RDLoaderPlugin loader = {
    .level = REDASM_API_LEVEL,
    .id = "dos",
    .name = "DOS Executable",
    .parse = dos::parse,
    .load = dos::load,
};

} // namespace dos
