#include "program.h"
#include "../utils/utils.h"
#include "mbyte.h"
#include <algorithm>

namespace redasm {

namespace {

template<typename T, typename Slice>
T* find_range(const Slice& s, RDAddress address, isize* index = nullptr) {
    auto res = slice_bsearch(
        &s, ct_inttoptr(address), +[](const RDAddress* key, const T* item) {
            auto addr = ct_ptrtoint(RDAddress, key);
            if(addr < item->start) return -1;
            if(addr >= item->end) return 1;
            return 0;
        });

    if(index) *index = res.index;
    if(res.found) return &slice_at(&s, res.index);
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

    RDSRegTree *regit, *tmp;
    hmap_foreach_safe(regit, tmp, &this->segmentregs, RDSRegTree, hnode) {
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

    isize index = -1;
    auto* fs = redasm::find_range<RDSegment>(this->segments, start, &index);
    if(fs) return false; // Overlap detected

    RDSegment s = {
        .name = utils::copy_str(name),
        .start = start,
        .end = end,
        .perm = perm,
        .bits = bits,
        .mem = rdbuffer_creatememory(end - start),
    };

    slice_insert(&this->segments, index, s);

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

    RDSRegTree* tree;
    hmap_get(tree, &this->segmentregs, ct_inttoptr(sreg), RDSRegTree, hnode,
             tree->sreg == sreg);

    if(tree) {
        RDSRange* r =
            rbtree_find_item(&tree->root, ct_inttoptr(start), RDSRange, rbnode);

        if(r) {
            // Check if we can extend or merge with previous range
            RDSRange* prevr = rbtree_prev_item(&r->rbnode, RDSRange, rbnode);

            if(prevr) {
                if(prevr->end >= start) { // Overlap or adjacency
                    if(RDRegValue_eq_val(prevr->val, val)) {
                        prevr->end = std::max(prevr->end, end); // Extend
                        return true;
                    }

                    // Split previous range
                    prevr->end = std::min(prevr->end, start);
                }
            }

            std::vector<RDSRange*> toerase, tosplit;

            // Check if we can merge with next range
            RDSRange* it = r;
            rbtree_foreach_from(it, &tree->root, RDSRange, rbnode) {
                if(RDRegValue_eq_val(it->val, val)) // Remove & continue merge
                    toerase.push_back(it);
                else if(it->start < end) {
                    tosplit.push_back(it);
                }
                else
                    break;
            }

            for(RDSRange* it : toerase) {
                end = std::max(end, it->end);
                rbtree_erase(&tree->root, &it->rbnode);
                delete it;
            }

            for(RDSRange* it : tosplit) {
                auto* n = new RDSRange{
                    .start = end,
                    .end = it->end,
                    .val = RDRegValue_some(val),
                };

                it->end = end;
                rbtree_insert(&tree->root, &n->rbnode);
            }
        }
    }
    else {
        tree = new RDSRegTree{.sreg = sreg};
        rbtree_init(&tree->root, sregtree_ncompare, sregtree_kcompare);
        hmap_set(&this->segmentregs, &tree->hnode, ct_inttoptr(sreg));
    }

    ct_assume(start < end);

    auto* r = new RDSRange{
        .start = start,
        .end = end,
        .val = RDRegValue_some(val),
    };

    rbtree_insert(&tree->root, &r->rbnode);
    return true;
}

bool Program::set_sreg(RDAddress address, int sreg,
                       const RDRegValue& val) { // NOLINT
    RDSRegTree* tree;
    hmap_get(tree, &this->segmentregs, ct_inttoptr(sreg), RDSRegTree, hnode,
             tree->sreg == sreg);
    if(!tree) return false;

    RDSRange* r =
        rbtree_find_item(&tree->root, ct_inttoptr(address), RDSRange, rbnode);
    if(!r) return false;

    if(address != r->start) {
        RDAddress oldend = r->end;
        r->end = address;

        auto* newr = new RDSRange{
            .start = address,
            .end = oldend,
            .val = val,
        };

        rbtree_insert(&tree->root, &newr->rbnode);
    }
    else
        r->val = val;

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

const RDSRange* Program::find_sreg_range(RDAddress address, int sreg) const {
    RDSRegTree* tree;
    hmap_get(tree, &this->segmentregs, ct_inttoptr(sreg), RDSRegTree, hnode,
             tree->sreg == sreg);

    if(tree) {
        return rbtree_find_item(&tree->root, ct_inttoptr(address), RDSRange,
                                rbnode);
    }

    return nullptr;
}

RDSegment* Program::find_segment(std::string_view name) {
    auto res = slice_bsearch(
        &this->segments, &name,
        +[](const std::string_view* key, const RDSegment* item) {
            return key->compare(item->name);
        });

    if(res.found) return &slice_at(&this->segments, res.index);
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
