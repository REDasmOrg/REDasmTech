#include "stream.h"
#include "../context.h"
#include "../state.h"

namespace redasm {

tl::optional<u8> Stream::at(usize idx) const {
    if(idx >= state::context->program.file->size())
        return tl::nullopt;

    return state::context->program.file->get_byte(idx);
}

usize Stream::size() const { return state::context->program.file->size(); }
const AbstractBuffer& Stream::buffer() const { return *state::context->program.file; }

} // namespace redasm
