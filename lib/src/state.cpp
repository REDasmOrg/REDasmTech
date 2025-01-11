#include "state.h"
#include "context.h"

namespace redasm::state {

const typing::BaseTypes& get_types() {
    if(state::context) return state::context->types;
    return state::BASETYPES;
}

} // namespace redasm::state
