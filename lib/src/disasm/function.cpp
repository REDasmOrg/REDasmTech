#include "function.h"
#include "../error.h"
#include <algorithm>

namespace redasm {

Function::Function(usize ep): entry{ep} {}

bool Function::contains(usize idx) const {
    return std::any_of(m_blocks.begin(), m_blocks.end(),
                       [idx](const BasicBlock& x) {
                           const auto [start, end] = x;
                           return idx >= start && idx <= end;
                       });
}

void Function::add_block(usize start, usize end) {
    assume(start != end);
    if(start > end)
        std::swap(start, end);

    BasicBlock bb{start, end};
    auto it = std::lower_bound(m_blocks.begin(), m_blocks.end(), bb,
                               [](const BasicBlock& a, const BasicBlock& b) {
                                   return a.first < b.first;
                               });

    m_blocks.insert(it, bb);
}

} // namespace redasm
