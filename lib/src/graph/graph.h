#pragma once

#include "../utils/object.h"
#include <redasm/graph.h>
#include <string>
#include <vector>

namespace redasm {

class Graph: public Object {
public:
    virtual void clear();
    void remove_edge(const RDGraphEdge* edge);
    void remove_node(RDGraphNode n);
    void push_edge(RDGraphNode source, RDGraphNode target);
    RDGraphNode push_node();
    std::string generate_dot() const;
    u32 hash() const;

    inline RDGraphNode set_root(RDGraphNode n) {
        m_root = n;
        return n;
    }

public:
    const RDGraphEdge* edge(RDGraphNode source, RDGraphNode target) const;
    usize outgoing(RDGraphNode n, const RDGraphEdge** edges) const;
    usize incoming(RDGraphNode n, const RDGraphEdge** edges) const;
    usize nodes(const RDGraphNode** nodes) const;
    usize edges(const RDGraphEdge** edges) const;

    inline RDGraphNode root() const { return m_root; }
    inline bool empty() const { return m_nodes.empty(); }

    inline bool contains_edge(RDGraphNode source, RDGraphNode target) const {
        return this->edge(source, target);
    }

protected:
    virtual std::string node_label(RDGraphNode n) const;
    void remove_outgoing_edges(RDGraphNode n);
    void remove_incoming_edges(RDGraphNode n);
    void remove_edges(RDGraphNode n);

protected:
    mutable std::vector<RDGraphEdge> m_incomings, m_outgoings;
    std::vector<RDGraphEdge> m_edges;
    std::vector<RDGraphNode> m_nodes;
    usize m_nodeid{0};
    RDGraphNode m_root{0};
};

} // namespace redasm
