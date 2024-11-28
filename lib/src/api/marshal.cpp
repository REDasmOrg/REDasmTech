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

RDValue to_c(const typing::Value& v) {
    return {
        .type = v.type,
        .list = api::to_c(&v.list),
        .listcount = v.list.size(),
        .dict = api::to_c(&v.dict),
        .dictcount = v.dict.size(),
        .str = v.str.c_str(),
        .u64_v = v.u64_v,
    };
}

} // namespace redasm::api
