#include "function.h"
#include <algorithm>

namespace redasm {

Function::Function(MIndex ep): entry{ep} {}

bool Function::contains(MIndex idx) const {
    return std::any_of(
        this->blocks.begin(), this->blocks.end(),
        [idx](const BasicBlock& x) { return idx >= x.start && idx <= x.end; });
}

RDGraphNode Function::try_add_block(MIndex start) {
    for(const BasicBlock& bb : this->blocks) {
        if(bb.start == start)
            return bb.node;
    }

    auto it = std::lower_bound(
        this->blocks.begin(), this->blocks.end(), start,
        [](const BasicBlock& a, MIndex b) { return a.start < b; });

    return this->blocks.emplace(it, this->graph.add_node(), start)->node;
}

Function::BasicBlock* Function::get_basic_block(RDGraphNode n) {
    for(BasicBlock& bb : this->blocks) {
        if(bb.node == n)
            return &bb;
    }

    return nullptr;
}

} // namespace redasm
