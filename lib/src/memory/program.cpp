#include "program.h"
#include "../internal/buffer_internal.h"
#include "../utils/utils.h"
#include "mbyte.h"
#include <algorithm>

namespace redasm {

namespace {

template<typename T>
bool range_overlaps(T s1, T e1, T s2, T e2) {
    return s1 < e2 && s2 < e1;
}

} // namespace

Program::Program() {
    this->regions = map_create(RDProgramRegion);

    map_setitemdel(this->regions, [](MapBucket* b) {
        auto* pr = reinterpret_cast<RDProgramRegion*>(b);
        vect_destroy(pr->value);
    });
}

Program::~Program() {
    for(RDSegment& s : this->segments) {
        rdbuffer_destroy(s.mem);
        delete[] s.name;
    }

    map_destroy(this->regions);
}

tl::optional<RDOffset> Program::to_offset(RDAddress address) const {
    auto it = std::ranges::lower_bound(
        this->mappings, address,
        [](RDAddress start, RDAddress addr) { return start < addr; },
        [](const FileMapping& m) { return m.base; });

    if(it != this->mappings.end() && address >= it->base &&
       address < it->base + it->length)
        return it->offset + (address - it->base);

    if(it != this->mappings.begin()) {
        --it;

        if(address >= it->base && address < it->base + it->length)
            return it->offset + (address - it->base);
    }

    return tl::nullopt;
}

tl::optional<RDAddress> Program::to_address(RDOffset offset) const {
    auto it = std::ranges::lower_bound(
        this->mappings, offset,
        [](RDOffset start, RDOffset off) { return start < off; },
        [](const FileMapping& m) { return m.offset; });

    if(it != this->mappings.end() && offset < it->offset + it->length)
        return it->base + (offset - it->offset);

    if(it != this->mappings.begin()) {
        --it;

        if(offset >= it->base && offset < it->offset + it->length)
            return it->base + (offset - it->offset);
    }

    return tl::nullopt;
}

bool Program::add_segment(std::string_view name, RDAddress start, RDAddress end,
                          u32 perm, u32 bits) {
    if(start >= end) return false;

    auto it = std::ranges::lower_bound(
        this->segments, start,
        [](RDAddress endaddr, RDAddress addr) { return endaddr <= addr; },
        [](const RDSegment& s) { return s.end; });

    if(it != this->segments.end() && it->start < end)
        return false; // Overlap detected

    RDSegment s = {
        .name = utils::copy_str(name),
        .start = start,
        .end = end,
        .perm = perm,
        .bits = bits,
        .mem = rdbuffer_creatememory(end - start),
    };

    this->segments.insert(it, s);

    for(const FileMapping& m : this->mappings) {
        usize mapend = m.base + m.length;

        if(m.base < end && mapend > start) {
            usize overlapstart = std::max(start, m.base);
            usize overlapend = std::min(end, mapend);
            usize segoff = overlapstart - start;
            usize fileoff = m.offset + (overlapstart - m.base);
            usize copylen = overlapend - overlapstart;

            for(usize i = fileoff; i < fileoff + copylen; i++)
                mbyte::set_byte(&s.mem->m_data[segoff++], this->file->data[i]);
        }
    }

    return true;
}

bool Program::map_file(RDOffset off, RDAddress start, RDAddress end) {
    if(start >= end) return false;

    usize n = end - start;
    if(off + n > this->file->length) return false;

    FileMapping m = {
        .base = start,
        .length = n,
        .offset = off,
    };

    for(RDSegment& s : this->segments) {
        if(start < s.end && end > s.start) {
            usize overlapstart = std::max(start, s.start);
            usize overlapend = std::min(end, s.end);
            usize segmentoff = overlapstart - s.start;
            usize fileoff = off + (overlapstart - start);
            usize copylen = overlapend - overlapstart;

            for(usize i = fileoff; i < fileoff + copylen; i++)
                mbyte::set_byte(&s.mem->m_data[segmentoff++],
                                this->file->data[i]);
        }
    }

    auto it = std::ranges::lower_bound(
        this->mappings, start,
        [](RDAddress endaddr, RDAddress addr) { return endaddr <= addr; },
        [](const FileMapping& m) { return m.base + m.length; });

    this->mappings.insert(it, m);
    return true;
}

RDSegment* Program::find_segment(RDAddress address) {
    auto it = std::ranges::lower_bound(
        this->segments, address,
        [](RDAddress start, RDAddress addr) { return start < addr; },
        [](const RDSegment& s) { return s.start; });

    if(it != this->segments.end() && address >= it->start && address < it->end)
        return std::addressof(*it);

    if(it != this->segments.begin()) {
        --it;

        if(address >= it->start && address < it->end)
            return std::addressof(*it);
    }

    return nullptr;
}

} // namespace redasm
