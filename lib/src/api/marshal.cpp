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

    auto address = ctx->index_to_address(arg.index);
    assume(address.has_value());
    s.address = *address;

    address = ctx->index_to_address(arg.endindex);
    assume(address.has_value());
    s.endaddress = *address;

    return s;
}

RDRef to_c(const AddressDetail::Ref& arg) {
    auto address = state::context->index_to_address(arg.index);
    assume(address.has_value());

    return {
        *address,
        arg.type,
    };
}

std::vector<RDRef> to_c(const std::vector<AddressDetail::Ref>& arg) {
    std::vector<RDRef> refs;
    refs.reserve(arg.size());

    for(const AddressDetail::Ref& r : arg)
        refs.push_back(api::to_c(r));

    return refs;
}

} // namespace redasm::api
