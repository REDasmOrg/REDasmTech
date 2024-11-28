#include "graph.h"
#include "../utils/hash.h"
#include <algorithm>

namespace redasm {

void Graph::clear() {
    m_incomings.clear();
    m_outgoings.clear();
    m_edges.clear();
    m_nodes.clear();
    m_nodeid = 0;
    m_root = {0};
}

void Graph::remove_edge(const RDGraphEdge* edge) {
    auto it = std::find_if(
        m_edges.begin(), m_edges.end(), [edge](const RDGraphEdge& e) {
            return std::tie(edge->src, edge->dst) == std::tie(e.src, e.dst);
        });

    if(it != m_edges.end())
        m_edges.erase(it);
}

void Graph::remove_node(RDGraphNode n) {
    auto it = std::find(m_nodes.begin(), m_nodes.end(), n);
    if(it == m_nodes.end())
        return;

    m_nodes.erase(it);
    this->remove_edges(n);
}

void Graph::add_edge(RDGraphNode src, RDGraphNode dst) {
    if(!this->contains_edge(src, dst))
        m_edges.push_back({src, dst});
}

RDGraphNode Graph::add_node() {
    RDGraphNode n = ++m_nodeid;
    m_nodes.push_back(n);
    return n;
}

std::string Graph::generate_dot() const {
    std::string s = "digraph G{\n";

    for(RDGraphNode n : m_nodes) {
        const RDGraphEdge* edges = nullptr;
        usize c = this->outgoing(n, &edges);

        for(usize i = 0; i < c; i++) {
            s += "\t\"" + this->node_label(edges[i].src) + "\"";
            s += " -> \"" + this->node_label(edges[i].dst) + "\";\n";
        }
    }

    s += "}";
    return s;
}

u32 Graph::hash() const { return hash::murmur3(this->generate_dot()); }

const RDGraphEdge* Graph::edge(RDGraphNode src, RDGraphNode dst) const {
    for(const RDGraphEdge& e : m_edges) {
        if((e.src == src) && (e.dst == dst))
            return &e;
    }

    return nullptr;
}

usize Graph::outgoing(RDGraphNode n, const RDGraphEdge** edges) const {
    m_outgoings.clear();

    for(const RDGraphEdge& e : m_edges) {
        if(e.src == n)
            m_outgoings.push_back(e);
    }

    if(edges)
        *edges = m_outgoings.data();
    return m_outgoings.size();
}

usize Graph::incoming(RDGraphNode n, const RDGraphEdge** edges) const {
    m_incomings.clear();

    for(const RDGraphEdge& e : m_edges) {
        if(e.dst == n)
            m_incomings.push_back(e);
    }

    if(edges)
        *edges = m_incomings.data();

    return m_incomings.size();
}

usize Graph::nodes(const RDGraphNode** nodes) const {
    if(nodes)
        *nodes = m_nodes.data();
    return m_nodes.size();
}

usize Graph::edges(const RDGraphEdge** edges) const {
    if(edges)
        *edges = m_edges.data();
    return m_edges.size();
}

std::string Graph::node_label(RDGraphNode n) const {
    return "#" + std::to_string(n);
}

void Graph::remove_outgoing_edges(RDGraphNode n) {
    this->outgoing(n, nullptr);

    for(const RDGraphEdge& e : m_outgoings)
        this->remove_edge(&e);
}

void Graph::remove_incoming_edges(RDGraphNode n) {
    this->incoming(n, nullptr);

    for(const RDGraphEdge& e : m_incomings)
        this->remove_edge(&e);
}

void Graph::remove_edges(RDGraphNode n) {
    for(usize i = 0; i < m_edges.size();) {
        const RDGraphEdge& e = m_edges[i];

        if(e.src == n)
            this->remove_edge(&e);
        else
            i++;
    }
}

} // namespace redasm
