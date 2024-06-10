#include "styledgraph.h"
#include <algorithm>

namespace redasm {

void StyledGraph::clear_layout() {
    m_nodeattributes.clear();
    m_edgeattributes.clear();
    m_areawidth = m_areaheight = 0;
}

void StyledGraph::clear() {
    DataGraph::clear();
    this->clear_layout();
}

void StyledGraph::set_color(const RDGraphEdge* e, const std::string& s) {
    m_edgeattributes[*e].color = s;
}

void StyledGraph::set_label(const RDGraphEdge* e, const std::string& s) {
    m_edgeattributes[*e].label = s;
}

void StyledGraph::set_routes(const RDGraphEdge* e, const RDGraphPoint* p,
                             usize n) {
    m_edgeattributes[*e].routes.resize(n);
    std::copy_n(p, n, m_edgeattributes[*e].routes.data());
}

void StyledGraph::set_arrow(const RDGraphEdge* e, const RDGraphPoint* p,
                            usize n) {
    m_edgeattributes[*e].arrow.resize(n);
    std::copy_n(p, n, m_edgeattributes[*e].arrow.data());
}

void StyledGraph::set_areawidth(int w) { m_areawidth = w; }
void StyledGraph::set_areaheight(int h) { m_areaheight = h; }
void StyledGraph::set_x(RDGraphNode n, int px) { m_nodeattributes[n].x = px; }
void StyledGraph::set_y(RDGraphNode n, int py) { m_nodeattributes[n].y = py; }
void StyledGraph::set_width(RDGraphNode n, int w) {
    m_nodeattributes[n].width = w;
}

void StyledGraph::set_height(RDGraphNode n, int h) {
    m_nodeattributes[n].height = h;
}

int StyledGraph::area_width() const { return m_areawidth; }
int StyledGraph::area_height() const { return m_areaheight; }

std::string_view StyledGraph::color(const RDGraphEdge* e) const {
    auto it = m_edgeattributes.find(*e);
    return (it != m_edgeattributes.end()) ? it->second.color
                                          : std::string_view{};
}

std::string_view StyledGraph::label(const RDGraphEdge* e) const {
    auto it = m_edgeattributes.find(*e);
    return (it != m_edgeattributes.end()) ? it->second.label
                                          : std::string_view{};
}

usize StyledGraph::routes(const RDGraphEdge* e,
                          const RDGraphPoint** path) const {
    auto it = m_edgeattributes.find(*e);
    if(it == m_edgeattributes.end())
        return 0;

    if(path)
        *path = it->second.routes.data();
    return it->second.routes.size();
}

usize StyledGraph::arrow(const RDGraphEdge* e,
                         const RDGraphPoint** path) const {
    auto it = m_edgeattributes.find(*e);
    if(it == m_edgeattributes.end())
        return 0;

    if(path)
        *path = it->second.arrow.data();
    return it->second.arrow.size();
}

int StyledGraph::x(RDGraphNode n) const {
    auto it = m_nodeattributes.find(n);
    return (it != m_nodeattributes.end()) ? it->second.x : 0;
}

int StyledGraph::y(RDGraphNode n) const {
    auto it = m_nodeattributes.find(n);
    return (it != m_nodeattributes.end()) ? it->second.y : 0;
}

int StyledGraph::width(RDGraphNode n) const {
    auto it = m_nodeattributes.find(n);
    return (it != m_nodeattributes.end()) ? it->second.width : 0;
}

int StyledGraph::height(RDGraphNode n) const {
    auto it = m_nodeattributes.find(n);
    return (it != m_nodeattributes.end()) ? it->second.height : 0;
}

} // namespace redasm
