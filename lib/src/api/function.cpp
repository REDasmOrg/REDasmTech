#include "../context.h"
#include "../memory/memory.h"
#include "../state.h"
#include "marshal.h"
#include <redasm/function.h>
#include <spdlog/spdlog.h>

RDFunction* rd_findfunction(RDAddress address) {
    spdlog::trace("rd_findfunction({:x})", address);
    redasm::Context* ctx = redasm::state::context;

    auto it = std::ranges::lower_bound(
        ctx->functions, address,
        [](RDAddress addr1, RDAddress addr2) { return addr1 < addr2; },
        [](const redasm::Function& f) { return f.address; });

    if(it != ctx->functions.end() && it->contains(address))
        return redasm::api::to_c(std::addressof(*it));

    if(it != ctx->functions.begin()) {
        --it;
        if(it->contains(address)) return redasm::api::to_c(std::addressof(*it));
    }

    if(it != ctx->functions.end() && it->contains(address))
        return redasm::api::to_c(std::addressof(*it));

    return nullptr;
}

RDAddress rdfunction_getentry(const RDFunction* self) {
    spdlog::trace("rd_functiongetentry({})", fmt::ptr(self));
    return redasm::api::from_c(self)->address;
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
    const redasm::Context* ctx = redasm::state::context;

    if(ctx) {
        const redasm::Function* f = redasm::api::from_c(self);
        const RDSegment* seg = ctx->program.find_segment(f->address);
        return seg && redasm::memory::has_flag(seg, f->address, BF_EXPORT);
    }

    return false;
}

bool rdfunction_contains(const RDFunction* self, RDAddress address) {
    spdlog::trace("rdfunction_contains({}, {:x})", fmt::ptr(self), address);

    return redasm::state::context &&
           redasm::api::from_c(self)->contains(address);
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
