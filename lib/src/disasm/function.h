#pragma once

#include "../graph/styledgraph.h"
#include "../types.h"
#include <vector>

namespace redasm {

struct Function {
    struct BasicBlock {
        BasicBlock(RDGraphNode n, MIndex s): node{n}, start{s}, end{s} {}

        RDGraphNode node;
        MIndex start;
        MIndex end;
        std::unordered_map<RDGraphNode, usize> theme;
    };

    using Blocks = std::vector<BasicBlock>;

    explicit Function(MIndex ep);
    bool contains(MIndex idx) const;
    RDGraphNode try_add_block(MIndex start);
    BasicBlock* get_basic_block(RDGraphNode n);

    inline const BasicBlock* get_basic_block(RDGraphNode n) const {
        return const_cast<Function*>(this)->get_basic_block(n);
    }

    MIndex entry;
    StyledGraph graph;
    Blocks blocks;
};

} // namespace redasm
