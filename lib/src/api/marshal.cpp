#include "marshal.h"
#include "../error.h"

namespace redasm::api {

RDFunctionBasicBlock to_c(const Function::BasicBlock* bb) {
    assume(bb);

    return {
        bb->start,
        bb->end,
    };
}

} // namespace redasm::api
