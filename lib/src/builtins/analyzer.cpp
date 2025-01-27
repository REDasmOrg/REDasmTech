#include "analyzer.h"
#include "../context.h"
#include "../plugins/pluginmanager.h"
#include "../state.h"
#include <redasm/analyzer.h>

namespace redasm::builtins {

namespace {

void do_autorename(RDAnalyzer*) {
    Context* ctx = state::context;

    for(const Function& f : ctx->functions) {
        if(Byte b = ctx->program.memory->at(f.index); !b.has(BF_FLOW)) {
            if(b.has(BF_JUMP)) {
                // Search for direct/indirect jumps
                for(const Database::Ref& ref : ctx->get_refs_from(f.index)) {
                    if(ref.type == CR_JUMP || ref.type == DR_READ) {
                        ctx->set_name(f.index, "_" + ctx->get_name(ref.index),
                                      SN_NOWARN);
                        break;
                    }
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

RDAnalyzerPlugin autorename_analyzer = {
    .level = REDASM_API_LEVEL,
    .id = "autorename",
    .name = "Autorename Nullsubs and Thunks",
    .flags = PF_LAST | AF_SELECTED,
    .order = 0,
    .is_enabled =
        [](const RDAnalyzerPlugin*) {
            const RDLoaderPlugin* ldr = state::context->loaderplugin;
            return !(ldr->flags & LF_NOAUTORENAME);
        },
    .execute = do_autorename,
};

} // namespace

void register_analyzers() {
    pm::register_analyzer(&autorename_analyzer, pm::NATIVE);
}

} // namespace redasm::builtins
