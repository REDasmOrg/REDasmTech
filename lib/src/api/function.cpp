#include "../context.h"
#include "../memory/memory.h"
#include "../state.h"
#include "marshal.h"
#include <redasm/function.h>
#include <spdlog/spdlog.h>

RDFunction* rd_findfunction(RDAddress address) {
    spdlog::trace("rd_findfunction({:x})", address);
    redasm::Context* ctx = redasm::state::context;
    return redasm::api::to_c(ctx->program.find_function(address));
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

const RDBasicBlock* rdfunction_getbasicblock(const RDFunction* self,
                                             RDGraphNode n) {
    spdlog::trace("rdfunction_getbasicblock({}, {})", fmt::ptr(self), n);
    return redasm::api::from_c(self)->get_basic_block(n);
}
