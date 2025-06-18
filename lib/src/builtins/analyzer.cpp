#include "analyzer.h"
#include "../context.h"
#include "../plugins/pluginmanager.h"
#include "../rdil/rdil.h"
#include "../state.h"
#include <limits>
#include <redasm/analyzer.h>

namespace redasm::builtins {

namespace {

void do_autorename(RDAnalyzer*) {
    Context* ctx = state::context;

    for(const Function& f : ctx->program.functions) {
        rdil::ILExprList el;
        rdil::decode(f.address, el);
        if(el.empty()) return;

        const RDILExpr* e = el.first();

        if(e->op == RDIL_GOTO) {
            if(e->u->op == RDIL_MEM) e = e->u;

            std::string n;

            if(e->u->op == RDIL_VAR)
                n = ctx->get_name(e->u->addr, false);
            else if(e->u->op == RDIL_CNST)
                n = ctx->get_name(e->u->u_cnst, false);

            if(!n.empty()) ctx->set_name(f.address, "_" + n, SN_NOWARN);
        }
        else if(e->op == RDIL_NOP || e->op == RDIL_RET)
            ctx->set_name(f.address, "nullsub", SN_ADDRESS);
    }
}

RDAnalyzerPlugin autorename_analyzer = {
    .level = REDASM_API_LEVEL,
    .id = "autorename",
    .name = "Autorename Nullsubs and Thunks",
    .flags = PF_LAST | AF_SELECTED,
    .order = std::numeric_limits<u32>::max(),
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
