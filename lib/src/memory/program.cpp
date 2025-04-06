#include "program.h"
#include "../utils/utils.h"
#include "mbyte.h"
#include <algorithm>

namespace redasm {

namespace {

template<typename T, typename Slice>
T* find_range(const Slice& s, RDAddress address) {
    auto res = slice_bsearch(
        &s, address, +[](const RDAddress* key, const T* item) {
            auto addr = reinterpret_cast<RDAddress>(key);
            if(addr < item->start) return -1;
            if(addr >= item->end) return 1;
            return 0;
        });

    if(res.found) return &s.data[res.index];
    return nullptr;
}

int sregtree_ncompare(const RBTreeNode* a, const RBTreeNode* b) {
    const auto* n1 = rbtree_item(a, RDSRange, rbnode);
    const auto* n2 = rbtree_item(b, RDSRange, rbnode);
    return n1->start - n2->start;
}

int sregtree_kcompare(const void* k, const RBTreeNode* n) {
    RDAddress addr = ct_ptrtoint(RDAddress, k);
    const auto* r = rbtree_item(n, RDSRange, rbnode);
    if(addr < r->start) return -1;
    if(addr >= r->end) return 1;
    return 0;
}

} // namespace

Program::Program() {
    slice_init(&this->segments, nullptr, nullptr);
    hmap_init(&this->segmentregs, nullptr);
}

Program::~Program() {
    RDSegment* segit;
    slice_foreach(segit, &this->segments) {
        rdbuffer_destroy(segit->mem);
        delete[] segit->name;
    }
    slice_destroy(&this->segments);

    RDSRegTree* regit;
    hmap_foreach(regit, &this->segmentregs, RDSRegTree, hnode) {
        RDSRange *rangeit, *tmp;
        rbtree_foreach_safe(rangeit, tmp, &regit->root, RDSRange, rbnode) {
            delete rangeit;
        }
        delete regit;
    }
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

    auto r = slice_bsearch(
        &this->segments, start,
        +[](const RDAddress* key, const RDSegment* val) -> int {
            return reinterpret_cast<RDAddress>(key) - val->end;
        });

    if(r.found) return false; // Overlap detected

    RDSegment s = {
        .name = utils::copy_str(name),
        .start = start,
        .end = end,
        .perm = perm,
        .bits = bits,
        .mem = rdbuffer_creatememory(end - start),
    };

    slice_insert(&this->segments, r.index, s);

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

bool Program::add_sreg_range(RDAddress start, RDAddress end, int sreg,
                             u64 val) {
    if(start >= end) return false;

    RDSRegTree* it;
    hmap_get(it, &this->segmentregs, RDSRegTree, hnode, sreg, it->sreg == sreg);

    if(!it) {
        it = new RDSRegTree{.sreg = sreg};
        rbtree_init(&it->root, sregtree_ncompare, sregtree_kcompare);

        auto* r = new RDSRange{
            .start = start,
            .end = end,
            .value = val,
        };

        rbtree_insert(&it->root, &r->rbnode);
        hmap_set(&this->segmentregs, &it->hnode, sreg);
        return true;
    }

    // uintptr_t idx = vect_bsearch(
    //     ranges, &start, +[](const RDAddress* start, const RDSegment* seg) {
    //         return *start < seg->start;
    //     });
    //
    // // Check if we can extend or merge with previous range
    // if(idx > 0) {
    //     RDSRegRange* prev = vect_ref(RDSRegRange, ranges, idx - 1);
    //
    //     if(prev->end >= start) { // Overlap or adjacency
    //         if(prev->value == val) {
    //             prev->end = std::max(prev->end, end); // Extend
    //             return true;
    //         }
    //         if(prev->end > start) { // Split the previous range
    //             prev->end = start;
    //
    //             vect_ins(RDSRegRange, ranges, idx,
    //                      {.start = start, .end = prev->end, .value = val});
    //         }
    //     }
    // }
    //
    // // Merge with next range if possible
    // while(idx < vect_length(ranges) &&
    //       vect_at(RDSRegRange, ranges, idx)->start <= end) {
    //     RDSRegRange* r = vect_ref(RDSRegRange, ranges, idx);
    //
    //     if(r->value == val) { // Remove and continue merging
    //         end = std::max(end, r->end);
    //         idx = vect_del(ranges, idx);
    //     }
    //     else if(r->start < end) { // Split the next range
    //         r->end = end;
    //         vect_ins(RDSRegRange, ranges, idx,
    //                  {.start = end, .end = r->end, .value = r->value});
    //     }
    //     else
    //         break;
    // }
    //
    // // Insert the new range
    // vect_ins(RDSRegRange, ranges, idx,
    //          {.start = start, .end = end, .value = val});

    return true;
}

void Program::set_sreg(RDAddress address, int sreg, u64 val) { // NOLINT
    // Vect(RDSRegRange)* ranges = map_get(RDSRegItem, this->segmentregs, sreg);
    // if(!ranges) return;
    //
    // RDSRegRange* it = std::ranges::lower_bound(
    //     vect_begin(*ranges), vect_end(*ranges), address,
    //     [](RDAddress end, RDAddress addr) { return end <= addr; },
    //     [](const RDSRegRange& r) { return r.end; });
    //
    // if(it == vect_end(*ranges) || it->start > address) return;
    //
    // if(it->start != address) {
    //     RDAddress oldend = it->end;
    //     it->end = address;
    //
    //     vect_ins_iter(RDSRegRange, *ranges, it + 1,
    //                   {.start = address, .end = oldend, .value = val});
    // }
    // else
    //     it->value = val;
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

    RDSegment* s;
    slice_foreach(s, &this->segments) {
        if(start < s->end && end > s->start) {
            usize overlapstart = std::max(start, s->start);
            usize overlapend = std::min(end, s->end);
            usize segmentoff = overlapstart - s->start;
            usize fileoff = off + (overlapstart - start);
            usize copylen = overlapend - overlapstart;

            for(usize i = fileoff; i < fileoff + copylen; i++)
                mbyte::set_byte(&s->mem->m_data[segmentoff++],
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

RDSRange* Program::find_sreg_range(RDAddress address, int r) {
    // RDSRegRange* range;
    // hmap_foreach_key(range, &this->segmentregs, RDSRegRange, hnode, r) {
    //     if(range->sreg == r && range->start >= address && address <
    //     range->end)
    //         return range;
    // }
    //
    return nullptr;
}

RDSegment* Program::find_segment(RDAddress address) { // NOLINT
    return redasm::find_range<RDSegment>(this->segments, address);
}

Function* Program::find_function(RDAddress address) {
    auto it = std::ranges::lower_bound(
        this->functions, address,
        [](RDAddress addr1, RDAddress addr2) { return addr1 < addr2; },
        [](const redasm::Function& f) { return f.address; });

    if(it != this->functions.end() && it->contains(address))
        return std::addressof(*it);

    if(it != this->functions.begin()) {
        --it;
        if(it->contains(address)) return std::addressof(*it);
    }

    return nullptr;
}

} // namespace redasm
