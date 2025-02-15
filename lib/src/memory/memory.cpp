#include "memory.h"
#include "../error.h"
#include "mbyte.h"

namespace redasm::memory {

namespace {

tl::optional<std::pair<RDAddress, RDAddress>> find_range(const RDSegment* self,
                                                         RDAddress addr) {
    assume(self);
    if(addr >= self->end) return tl::nullopt;

    // Single item range
    if(memory::has_flag(self, addr, BF_START | BF_END))
        return std::make_pair(addr, addr);

    RDAddress rstart = addr, rend = addr;

    // Traverse backward to find the range start
    while(rstart > 0 && memory::has_flag(self, rstart, BF_CONT))
        rstart--;

    if(rstart >= self->end || !memory::has_flag(self, rstart, BF_START))
        return tl::nullopt; // Not part of a valid range

    // Traverse forward to find the range end
    while(rend < self->end && (memory::has_flag(self, rend, BF_START) ||
                               (memory::has_flag(self, rend, BF_CONT) &&
                                !memory::has_flag(self, rend, BF_END)))) {
        rend++;
    }

    if(rend >= self->end || !memory::has_flag(self, rend, BF_END))
        return tl::nullopt; // Not part of a valid range

    assume(rstart <= rend);
    return std::make_pair(rstart, rend);
}

} // namespace

usize get_length(const RDSegment* self, RDAddress address) {
    if(auto r = memory::find_range(self, address); r)
        return r->second - r->first + 1;
    return 0;
}

tl::optional<RDAddress> get_next(const RDSegment* self, RDAddress address) {
    if(address >= self->end) return tl::nullopt;

    if(usize len = memory::get_length(self, address); len > 0)
        return address + len;
    return tl::nullopt;
}

bool is_unknown(const RDSegment* self, RDAddress address) {
    return mbyte::is_unknown(self->mem->m_data[address - self->start]);
}

bool has_common(const RDSegment* self, RDAddress address) {
    return mbyte::has_common(self->mem->m_data[address - self->start]);
}

bool has_byte(const RDSegment* self, RDAddress address) {
    return mbyte::has_byte(self->mem->m_data[address - self->start]);
}

bool has_flag(const RDSegment* self, RDAddress address, u32 f) {
    return mbyte::has_flag(self->mem->m_data[address - self->start], f);
}

void set_flag(RDSegment* self, RDAddress address, u32 f, bool b) {
    mbyte::set_flag(&self->mem->m_data[address - self->start], f, b);
}

void clear(RDSegment* self, RDAddress address) {
    mbyte::clear(&self->mem->m_data[address - self->start]);
}

void set_n(RDSegment* self, RDAddress address, usize n, u32 flags) {
    assume(self);
    if(!n) return;

    RDAddress end = std::min(address + n, self->end);
    memory::set_flag(self, address, flags | BF_START);

    for(RDAddress addr = address + 1; addr < end - 1; addr++)
        memory::set_flag(self, addr, flags | BF_CONT);

    if(n > 1) memory::set_flag(self, end - 1, flags | BF_CONT);
    memory::set_flag(self, end - 1, flags | BF_END);
}

void unset_n(RDSegment* self, RDAddress address, usize n) {
    assume(self);
    RDAddress end = std::min(address + n, self->end);

    for(RDAddress addr = address; addr < end; addr++) {
        if(memory::has_flag(self, addr, BF_START) ||
           memory::has_flag(self, addr, BF_CONT)) {
            auto r = memory::find_range(self, addr);
            assume(r);

            // Unset the overlapping range
            for(RDAddress raddr = r->first; raddr <= r->second; raddr++)
                memory::clear(self, raddr);

            // Skip to the end of the cleared range
            addr = r->second;
        }
        else
            memory::clear(self, addr);
    }
}

} // namespace redasm::memory
