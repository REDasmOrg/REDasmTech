#pragma once

#include <redasm/graph.h>
#include <string>
#include <vector>

namespace redasm {

class Graph {
public:
    using Nodes = std::vector<RDGraphNode>;
    using Edges = std::vector<RDGraphEdge>;

public:
    virtual ~Graph() = default;
    virtual void clear();
    void remove_edge(const RDGraphEdge* edge);
    void remove_node(RDGraphNode n);
    void add_edge(RDGraphNode source, RDGraphNode target);
    RDGraphNode add_node();
    std::string generate_dot() const;
    u32 hash() const;

    RDGraphNode set_root(RDGraphNode n) {
        m_root = n;
        return n;
    }

public:
    const RDGraphEdge* edge(RDGraphNode src, RDGraphNode dst) const;
    usize outgoing(RDGraphNode n, const RDGraphEdge** edges) const;
    usize incoming(RDGraphNode n, const RDGraphEdge** edges) const;
    usize nodes(const RDGraphNode** nodes) const;
    usize edges(const RDGraphEdge** edges) const;
    const Nodes& nodes() const { return m_nodes; }
    const Edges& edges() const { return m_edges; }
    RDGraphNode root() const { return m_root; }
    bool empty() const { return m_nodes.empty(); }

    bool contains_edge(RDGraphNode src, RDGraphNode dst) const {
        return this->edge(src, dst);
    }

protected:
    virtual std::string node_label(RDGraphNode n) const;
    void remove_outgoing_edges(RDGraphNode n);
    void remove_incoming_edges(RDGraphNode n);
    void remove_edges(RDGraphNode n);

protected:
    mutable Edges m_incomings, m_outgoings;
    Edges m_edges;
    Nodes m_nodes;
    usize m_nodeid{0};
    RDGraphNode m_root{0};
};

} // namespace redasm
