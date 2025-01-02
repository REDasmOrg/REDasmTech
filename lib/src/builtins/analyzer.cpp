#include "analyzer.h"
#include "../api/internal/analyzer.h"
#include "../api/internal/memory.h"
#include "../api/internal/redasm.h"
#include "../context.h"
#include "../state.h"
#include <limits>

namespace redasm::builtins {

namespace {

void strings_step(RDAddress& address, const RDMemoryInfo& mi) {
    RDStringResult sr;

    while(address < mi.end) {
        if(api::internal::check_string(address, &sr)) {
            auto index = api::internal::address_to_index(address);

            if(index) {
                Byte b = state::context->memory->at(*index);

                if(b.is_unknown())
                    api::internal::set_typename(address, sr.type);
            }

            address += sr.totalsize;
        }
        else
            address++;
    }
}

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
        .isenabled = [](const RDAnalyzer*) { return true; },
        .execute = do_autorename,
    };

    api::internal::register_analyzer(autorename);

    RDAnalyzer strings{
        .id = "strings",
        .name = "Search and mark all strings",
        .order = std::numeric_limits<u32>::max(),
        .isenabled = [](const RDAnalyzer*) { return true; },
    };

    strings.execute = [](const RDAnalyzer*) {
        RDMemoryInfo mi;
        api::internal::memory_info(&mi);

        RDAddress address = mi.start;

        while(address < mi.end)
            strings_step(address, mi);
    };

    api::internal::register_analyzer(strings);
}

} // namespace redasm::builtins
