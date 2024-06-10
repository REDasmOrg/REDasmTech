#pragma once

#include "datagraph.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace redasm {

using GraphPoints = std::vector<RDGraphPoint>;

class StyledGraph: public DataGraph {
private:
    struct NodeAttributes {
        int x, y, width, height;
    };

    struct EdgeAttributes {
        std::string label, color;
        GraphPoints routes, arrow;
    };

public:
    void clear_layout();
    void clear() override;

public: // Write
    void set_color(const RDGraphEdge* e, const std::string& s);
    void set_label(const RDGraphEdge* e, const std::string& s);
    void set_routes(const RDGraphEdge* e, const RDGraphPoint* p, usize n);
    void set_arrow(const RDGraphEdge* e, const RDGraphPoint* p, usize n);
    void set_areawidth(int w);
    void set_areaheight(int h);
    void set_x(RDGraphNode n, int px);
    void set_y(RDGraphNode n, int py);
    void set_width(RDGraphNode n, int w);
    void set_height(RDGraphNode n, int h);

public: // Read
    std::string_view color(const RDGraphEdge* e) const;
    std::string_view label(const RDGraphEdge* e) const;
    usize routes(const RDGraphEdge* e, const RDGraphPoint** path) const;
    usize arrow(const RDGraphEdge* e, const RDGraphPoint** path) const;
    int area_width() const;
    int area_height() const;
    int x(RDGraphNode n) const;
    int y(RDGraphNode n) const;
    int width(RDGraphNode n) const;
    int height(RDGraphNode n) const;

private:
    std::unordered_map<RDGraphNode, NodeAttributes> m_nodeattributes;
    std::unordered_map<RDGraphEdge, EdgeAttributes> m_edgeattributes;
    int m_areawidth{0}, m_areaheight{0};
};

} // namespace redasm
