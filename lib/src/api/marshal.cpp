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

    auto address = ctx->index_to_address(arg.index);
    assume(address.has_value());
    s.startaddr = *address;

    address = ctx->index_to_address(arg.endindex);
    assume(address.has_value());
    s.endaddr = *address;

    return s;
}

RDRef to_c(const Database::Ref& arg) {
    auto address = state::context->index_to_address(arg.index);
    assume(address.has_value());

    return {
        *address,
        arg.type,
    };
}

std::vector<RDRef> to_c(const std::vector<Database::Ref>& arg) {
    std::vector<RDRef> refs;
    refs.reserve(arg.size());

    for(Database::Ref r : arg)
        refs.push_back(api::to_c(r));

    return refs;
}

RDFunctionBasicBlock to_c(const Function::BasicBlock* bb) {
    assume(bb);

    auto startaddress = state::context->index_to_address(bb->start);
    assume(startaddress.has_value());

    auto endaddress = state::context->index_to_address(bb->end);
    assume(endaddress.has_value());

    return {
        *startaddress,
        *endaddress,
    };
}

} // namespace redasm::api
