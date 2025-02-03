#include "analyzer.h"
#include "../context.h"
#include "../memory/memory.h"
#include "../plugins/pluginmanager.h"
#include "../state.h"
#include "../utils/utils.h"
#include <redasm/analyzer.h>

namespace redasm::builtins {

namespace {

void do_autorename(RDAnalyzer*) {
    Context* ctx = state::context;

    for(const Function& f : ctx->functions) {
        const RDSegment* seg = ctx->program.find_segment(f.address);

        if(seg && !memory::has_flag(seg, f.address, BF_FLOW)) {
            if(memory::has_flag(seg, f.address, BF_JUMP)) {
                // Search for direct/indirect jumps
                for(const RDRef& ref : ctx->get_refs_from(f.address)) {
                    if(ref.type == CR_JUMP || ref.type == DR_READ) {
                        ctx->set_name(f.address,
                                      "_" + ctx->get_name(ref.address),
                                      SN_NOWARN);
                        break;
                    }
                }
            }
            else if(memory::has_flag(seg, f.address, BF_COMMENT)) {
                ctx->set_name(
                    f.address,
                    "nullsub_" + utils::to_hex<std::string>(f.address), 0);
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
