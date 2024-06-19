#include "function.h"
#include "../../context.h"
#include "../../state.h"
#include "../marshal.h"
#include <algorithm>
#include <spdlog/spdlog.h>

namespace redasm::api::internal {

RDFunction* get_function(RDAddress address) {
    spdlog::trace("get_function({:x})", address);

    Context* ctx = state::context;

    if(auto idx = ctx->address_to_index(address); idx) {
        auto it = std::lower_bound(
            ctx->functions.begin(), ctx->functions.end(), *idx,
            [](const Function& f, usize ep) { return f.entry < ep; });

        if(it != ctx->functions.end() && it->entry == *idx)
            return api::to_c(std::addressof(*it));
    }

    return nullptr;
}

RDGraph* function_getgraph(RDFunction* self) {
    spdlog::trace("function_getgraph({})", fmt::ptr(self));
    return api::to_c(&api::from_c(self)->graph);
}

bool function_contains(const RDFunction* self, RDAddress address) {
    spdlog::trace("function_contains({}, {:x})", fmt::ptr(self), address);

    if(auto idx = state::context->address_to_index(address); idx)
        return api::from_c(self)->contains(*idx);

    return false;
}

bool function_getbasicblock(const RDFunction* self, RDGraphNode n,
                            RDFunctionBasicBlock* bb) {
    spdlog::trace("function_getbasicblock({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(bb));
    const Function* f = api::from_c(self);

    if(const auto* res = f->get_basic_block(n); res) {
        if(bb)
            *bb = api::to_c(res);
        return true;
    }

    return false;
}

} // namespace redasm::api::internal
