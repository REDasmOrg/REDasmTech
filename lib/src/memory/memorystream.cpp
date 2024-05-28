#include "memorystream.h"
#include "../context.h"
#include "../state.h"

namespace redasm {

tl::optional<u8> MemoryStream::at(usize absidx) const {
    Byte b = state::context->memory->at(absidx - this->index_base());
    if(b.has_byte())
        return b.byte();
    return tl::nullopt;
}

usize MemoryStream::size() const { return state::context->memory->size(); }

const AbstractBuffer& MemoryStream::buffer() const {
    return *state::context->memory;
}

usize MemoryStream::index_base() const { return state::context->baseaddress; }

} // namespace redasm
