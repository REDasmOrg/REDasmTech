#include "stream.h"
#include "../context.h"
#include "../state.h"

namespace redasm {

tl::optional<u8> Stream::at(usize idx) const {
    if(idx >= state::context->file->size())
        return tl::nullopt;

    return state::context->file->get_byte(idx);
}

usize Stream::size() const { return state::context->file->size(); }
const AbstractBuffer& Stream::buffer() const { return *state::context->file; }

} // namespace redasm
