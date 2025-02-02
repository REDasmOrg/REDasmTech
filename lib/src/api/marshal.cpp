#include "marshal.h"
#include "../context.h"
#include "../error.h"
#include "../segment.h"
#include "../state.h"

namespace redasm::api {

RDSegment to_c(const Segment& arg) {
    const Context* ctx = state::context;

    RDSegment s;
    s.name = arg.name;
    s.perm = arg.perm;
    s.startoff = arg.offset;
    s.endoff = arg.endoffset;

    // auto address = ctx->index_to_address(arg.index);
    // assume(address.has_value());
    // s.startaddr = *address;
    //
    // address = ctx->index_to_address(arg.endindex);
    // assume(address.has_value());
    // s.endaddr = *address;

    return s;
}

RDFunctionBasicBlock to_c(const Function::BasicBlock* bb) {
    assume(bb);

    return {
        bb->start,
        bb->end,
    };
}

} // namespace redasm::api
