#include "marshal.h"
#include "../context.h"
#include "../error.h"
#include "../segment.h"
#include "../state.h"

namespace redasm::api {

RDSegment to_c(const Segment& arg) {
    const Context* ctx = state::context;

    RDSegment s;
    s.name = arg.name.c_str();
    s.type = arg.type;
    s.offset = arg.offset;
    s.endoffset = arg.endoffset;

    auto idx = ctx->index_to_address(arg.index);
    assume(idx.has_value());
    s.address = *idx;

    idx = ctx->index_to_address(arg.endindex);
    assume(idx.has_value());
    s.endaddress = *idx;

    return s;
}

} // namespace redasm::api
