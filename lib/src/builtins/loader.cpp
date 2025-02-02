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
    .accept = [](const RDLoaderPlugin*,
                 const RDLoaderRequest*) { return true; },

    .load =
        [](RDLoader*, RDBuffer* file) {
            usize n = rdbuffer_getlength(file);
            rd_mapfile_n(0, 0, n);
            rd_addsegment_n("BINARY", 0, n, SP_RWX, rd_countbits(n));
            return true;
        },
};

} // namespace

void register_loaders() { pm::register_loader(&binary_loader, pm::NATIVE); }

} // namespace redasm::builtins
