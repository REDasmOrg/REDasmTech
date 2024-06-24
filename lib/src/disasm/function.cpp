#include "function.h"
#include "../error.h"
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

RDThemeKind Function::get_theme(const RDGraphEdge& e) const {
    const BasicBlock* bb = this->get_basic_block(e.src);

    if(bb) {
        for(const auto& [dst, theme] : bb->theme) {
            if(e.dst == dst)
                return theme;
        }
    }

    return THEME_DEFAULT;
}

void Function::jmp(RDGraphNode src, RDGraphNode dst, RDThemeKind theme) {
    BasicBlock* bb = this->get_basic_block(src);
    assume(bb);
    bb->theme[dst] = theme;
    this->graph.add_edge(src, dst);
}

} // namespace redasm
