#include "datagraph.h"

namespace redasm {

void DataGraph::clear() {
    Graph::clear();
    m_data.clear();
}

const RDGraphData* DataGraph::data(RDGraphNode n) const {
    auto it = m_data.find(n);
    return (it != m_data.end()) ? std::addressof(it->second) : nullptr;
}

void DataGraph::set_data(RDGraphNode n, uptr val) { m_data[n].nu_data = val; }

void DataGraph::set_data(RDGraphNode n, iptr val) { m_data[n].ns_data = val; }

void DataGraph::set_data(RDGraphNode n, const char* val) {
    m_data[n].s_data = val;
}

void DataGraph::set_data(RDGraphNode n, void* val) { m_data[n].p_data = val; }

} // namespace redasm
