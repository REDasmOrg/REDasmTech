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
            [](const Function& f, usize ep) { return f.index < ep; });

        if(it != ctx->functions.end() && it->index == *idx)
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

RDAddress function_getentry(const RDFunction* self) {
    spdlog::trace("function_getentry({})", fmt::ptr(self));
    auto address = state::context->index_to_address(api::from_c(self)->index);
    assume(address.has_value());
    return *address;
}

RDThemeKind function_gettheme(const RDFunction* self, const RDGraphEdge* e) {
    spdlog::trace("function_gettheme({})", fmt::ptr(self), fmt::ptr(self));

    if(e)
        return api::from_c(self)->get_theme(*e);
    return THEME_DEFAULT;
}

bool function_isexport(const RDFunction* self) {
    spdlog::trace("function_isexport({})", fmt::ptr(self));

    if(state::context) {
        const Function* f = api::from_c(self);
        return state::context->memory->at(f->index).has(BF_EXPORT);
    }

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
