#include "loader.h"
#include "../plugins/pluginmanager.h"
#include <redasm/loader.h>
#include <redasm/memory.h>

namespace redasm::builtins {

namespace {

RDLoaderPlugin binary_loader = {
    .level = REDASM_API_LEVEL,
    .id = "binary",
    .name = "Binary",
    .flags = PF_LAST,
    .parse = [](RDLoader*, const RDLoaderRequest*) { return true; },

    .load =
        [](RDLoader*, RDBuffer* file) {
            rd_mapfile_n(0, 0, file->length);
            rd_addsegment_n("BINARY", 0, file->length, SP_RWX,
                            rd_countbits(file->length));
            return true;
        },
};

} // namespace

void register_loaders() { pm::register_loader(&binary_loader, pm::NATIVE); }

} // namespace redasm::builtins
