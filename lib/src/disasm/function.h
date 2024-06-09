#pragma once

#include <redasm/types.h>
#include <vector>

namespace redasm {

struct Function {
    using BasicBlock = std::pair<usize, usize>;
    using Blocks = std::vector<BasicBlock>;

    explicit Function(usize ep);
    bool contains(usize idx) const;
    void add_block(usize start, usize end);
    [[nodiscard]] inline const Blocks& blocks() const { return m_blocks; }

    usize entry;

private:
    Blocks m_blocks;
};

} // namespace redasm
