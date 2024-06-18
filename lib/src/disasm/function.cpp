#include "function.h"
#include "../error.h"
#include <algorithm>

namespace redasm {

Function::Function(MIndex ep): entry{ep} {}

bool Function::contains(MIndex idx) const {
    return std::any_of(
        m_blocks.begin(), m_blocks.end(),
        [idx](const BasicBlock& x) { return idx >= x.start && idx <= x.end; });
}

RDGraphNode Function::try_add_block(MIndex start) {
    for(RDGraphNode n : this->graph.nodes()) {
        const RDGraphData* data = this->graph.data(n);
        assume(data);
        assume(data->nu_data < m_blocks.size());

        if(m_blocks[data->nu_data].start == start)
            return n;
    }

    usize idx = m_blocks.size();
    m_blocks.emplace_back(start);
    return this->graph.add_datanode(static_cast<uptr>(idx));
}

Function::BasicBlock& Function::get_basic_block(RDGraphNode n) {
    const RDGraphData* data = this->graph.data(n);
    assume(data);

    auto idx = static_cast<usize>(data->nu_data);
    assume(idx < m_blocks.size());
    return m_blocks[idx];
}

} // namespace redasm
