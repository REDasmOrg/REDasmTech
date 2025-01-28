#include "../context.h"
#include "../state.h"
#include "marshal.h"
#include <algorithm>
#include <redasm/function.h>
#include <spdlog/spdlog.h>

RDFunction* rd_getfunction(RDAddress address) {
    spdlog::trace("rd_getfunction({:x})", address);
    redasm::Context* ctx = redasm::state::context;

    if(auto idx = ctx->address_to_index(address); idx) {
        auto it = std::lower_bound(
            ctx->functions.begin(), ctx->functions.end(), *idx,
            [](const redasm::Function& f, usize ep) { return f.index < ep; });

        if(it != ctx->functions.end() && it->index == *idx)
            return redasm::api::to_c(std::addressof(*it));
    }

    return nullptr;
}

RDAddress rdfunction_getentry(const RDFunction* self) {
    spdlog::trace("rd_functiongetentry({})", fmt::ptr(self));
    auto address = redasm::state::context->index_to_address(
        redasm::api::from_c(self)->index);
    assume(address.has_value());
    return *address;
}

RDGraph* rdfunction_getgraph(RDFunction* self) {
    spdlog::trace("rdfunction_getgraph({})", fmt::ptr(self));
    return redasm::api::to_c(&redasm::api::from_c(self)->graph);
}

RDThemeKind rdfunction_gettheme(const RDFunction* self,
                                const RDGraphEdge* edge) {
    spdlog::trace("rd_functiongettheme({})", fmt::ptr(self), fmt::ptr(self));
    if(edge) return redasm::api::from_c(self)->get_theme(*edge);
    return THEME_DEFAULT;
}

bool rdfunction_isexport(const RDFunction* self) {
    spdlog::trace("rdfunction_isexport({})", fmt::ptr(self));

    if(redasm::state::context) {
        const redasm::Function* f = redasm::api::from_c(self);
        return redasm::state::context->program.memory->at(f->index).has(
            BF_EXPORT);
    }

    return false;
}

bool rdfunction_contains(const RDFunction* self, RDAddress address) {
    spdlog::trace("rdfunction_contains({}, {:x})", fmt::ptr(self), address);

    if(redasm::state::context) {
        if(auto idx = redasm::state::context->address_to_index(address); idx)
            return redasm::api::from_c(self)->contains(*idx);
    }

    return false;
}

bool rdfunction_getbasicblock(const RDFunction* self, RDGraphNode n,
                              RDFunctionBasicBlock* bb) {
    spdlog::trace("rdfunction_getbasicblock({}, {}, {})", fmt::ptr(self), n,
                  fmt::ptr(bb));
    const redasm::Function* f = redasm::api::from_c(self);

    if(const auto* res = f->get_basic_block(n); res) {
        if(bb) *bb = redasm::api::to_c(res);
        return true;
    }

    return false;
}
