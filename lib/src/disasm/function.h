#pragma once

#include "../graph/styledgraph.h"
#include "../types.h"
#include <redasm/theme.h>
#include <vector>

namespace redasm {

struct Function {
    struct BasicBlock {
        BasicBlock(RDGraphNode n, RDAddress s): node{n}, start{s}, end{s} {}

        RDGraphNode node;
        RDAddress start;
        RDAddress end;
        std::unordered_map<RDGraphNode, RDThemeKind> theme;
    };

    using Blocks = std::vector<BasicBlock>;

    explicit Function(RDAddress ep);
    bool contains(RDAddress address) const;
    RDGraphNode try_add_block(RDAddress start);
    BasicBlock* get_basic_block(RDGraphNode n);
    RDThemeKind get_theme(const RDGraphEdge& e) const;

    void jmp(RDGraphNode src, RDGraphNode dst,
             RDThemeKind theme = THEME_DEFAULT);

    void jmp_true(RDGraphNode src, RDGraphNode dst) {
        this->jmp(src, dst, THEME_SUCCESS);
    }

    void jmp_false(RDGraphNode src, RDGraphNode dst) {
        this->jmp(src, dst, THEME_FAIL);
    }

    const BasicBlock* get_basic_block(RDGraphNode n) const {
        return const_cast<Function*>(this)->get_basic_block(n);
    }

    MIndex address;
    StyledGraph graph;
    Blocks blocks;
};

using FunctionList = std::vector<Function>;

} // namespace redasm
