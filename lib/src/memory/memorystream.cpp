#include "memorystream.h"
#include "../context.h"
#include "../state.h"

namespace redasm {

tl::optional<u8> MemoryStream::at(usize absidx) const {
    Byte b =
        state::context->program.memory_old->at(absidx - this->index_base());
    if(b.has_byte()) return b.byte();
    return tl::nullopt;
}

usize MemoryStream::size() const {
    return state::context->program.memory_old->size();
}

const AbstractBuffer& MemoryStream::buffer() const {
    return *state::context->program.memory_old;
}

usize MemoryStream::index_base() const { return 0; }

} // namespace redasm
