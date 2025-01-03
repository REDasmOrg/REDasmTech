#include "analyzer.h"
#include "../api/internal/analyzer.h"
#include "../context.h"
#include "../state.h"

namespace redasm::builtins {

namespace {

void do_autorename(const RDAnalyzer*) {
    Context* ctx = state::context;

    for(const Function& f : ctx->functions) {
        if(Byte b = ctx->memory->at(f.index); !b.has(BF_FLOW)) {
            if(b.has(BF_JUMP)) {
                Database::RefList refs =
                    ctx->get_refs_from_type(f.index, CR_JUMP);

                if(refs.size() == 1) {
                    ctx->set_name(f.index,
                                  "_" + ctx->get_name(refs.front().index), 0);
                }
            }
            else if(b.has(BF_CODE)) {
                ctx->index_to_address(f.index).map([&](RDAddress x) {
                    ctx->set_name(f.index, "nullsub_" + ctx->to_hex(x), 0);
                });
            }
        }
    }
}

} // namespace

void register_analyzers() {
    RDAnalyzer autorename{
        .id = "autorename",
        .name = "Autorename Nullsubs and Thunks",
        .flags = ANA_SELECTED,
        .order = 0,
        .isenabled =
            [](const RDAnalyzer*) {
                const RDLoader* ldr = state::context->loader;
                return !(ldr->flags & LF_NOAUTORENAME);
            },
        .execute = do_autorename,
    };

    api::internal::register_analyzer(autorename);
}

} // namespace redasm::builtins
