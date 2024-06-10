#pragma once

#include "graph.h"
#include <unordered_map>

namespace redasm {

class DataGraph: public Graph {
public:
    void clear() override;
    const RDGraphData* data(RDGraphNode n) const;
    void set_data(RDGraphNode n, uintptr_t val);
    void set_data(RDGraphNode n, intptr_t val);
    void set_data(RDGraphNode n, const char* val);
    void set_data(RDGraphNode n, void* val);

    template<typename T>
    RDGraphNode push_datanode(T val) {
        auto n = this->push_node();
        this->set_data(n, val);
        return n;
    }

private:
    std::unordered_map<RDGraphNode, RDGraphData> m_data;
};

} // namespace redasm
