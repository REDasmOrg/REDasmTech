#include "layeredlayout.h"
#include <queue>
#include <unordered_set>

namespace redasm {

namespace {

constexpr int LLAYOUT_PADDING = 16;
constexpr int LLAYOUT_PADDING_DIV2 = (LLAYOUT_PADDING / 2);
// constexpr int LLAYOUT_PADDING_DIV4 = (LLAYOUT_PADDING / 4);
constexpr int LLAYOUT_NODE_PADDING = (2 * LLAYOUT_PADDING);

} // namespace

LayeredLayout::LayeredLayout(StyledGraph* graph, int type)
    : m_graph{graph}, m_layouttype{type} {}

bool LayeredLayout::execute() {
    if(!m_graph->root())
        return false;

    m_blocks.clear();
    m_colx.clear();
    m_rowy.clear();
    m_rowedgey.clear();
    m_colwidth.clear();
    m_rowheight.clear();
    m_coledgecount.clear();
    m_rowedgey.clear();
    m_blockorder.clear();
    m_horizedges.clear();
    m_vertedges.clear();
    m_edgevalid.clear();

    this->create_blocks(); // Create render nodes
    this->make_acyclic();  // Construct acyclic graph where each node is used as
                           // an edge exactly once

    this->compute_layout(
        m_blocks[m_graph->root()]); // Compute graph layout from bottom up

    this->prepare_edge_routing(); // Prepare edge routing
    this->perform_edge_routing(); // Perform edge routing
    this->compute_edge_count();   // Compute edge counts for each row and column
    this->compute_row_column_sizes();     // Compute row and column sizes
    this->compute_row_column_positions(); // Compute row and column positions
    this->compute_node_positions();       // Compute node positions
    this->precompute_edge_coordinates();  // Precompute coordinates for edges
    return true;
}

void LayeredLayout::create_blocks() {
    const RDGraphNode* nodes = nullptr;
    size_t c = m_graph->nodes(&nodes);

    for(size_t i = 0; i < c; i++) {
        RDGraphNode n = nodes[i];
        m_graph->set_height(n,
                            m_graph->height(n) +
                                LLAYOUT_NODE_PADDING); // Pad Node

        m_blocks[n] = LLBlock(n, m_graph->width(n), m_graph->height(n));
    }

    // Populate incoming lists
    for(const auto& item : m_blocks) {
        const LLBlock& block = item.second;

        const RDGraphEdge* edges = nullptr;
        c = m_graph->outgoing(item.first, &edges);

        for(size_t i = 0; i < c; i++) {
            const RDGraphEdge& e = edges[i];
            m_blocks[e.target].incoming.push_back(block.node);
        }
    }
}

void LayeredLayout::make_acyclic() {
    // Construct acyclic graph where each node is used as an edge exactly once
    std::unordered_set<RDGraphNode> visited;
    std::queue<RDGraphNode> queue;
    bool changed = true;

    visited.insert(m_graph->root());
    queue.push(m_blocks[m_graph->root()].node);

    while(changed) {
        changed = false;

        // First pick nodes that have single entry points
        while(!queue.empty()) {
            LLBlock& block = m_blocks[queue.front()];
            queue.pop();
            m_blockorder.push_back(block.node);

            const RDGraphEdge* edges = nullptr;
            size_t c = m_graph->outgoing(block.node, &edges);

            for(size_t i = 0; i < c; i++) {
                const RDGraphEdge& e = edges[i];
                if(visited.count(e.target))
                    continue;

                // If node has no more unseen incoming edges, add it to the
                // graph layout now
                if(m_blocks[e.target].incoming.size() == 1) {
                    LayeredLayout::remove_from_deque(
                        m_blocks[e.target].incoming, block.node);
                    block.newoutgoing.push_back(e.target);
                    queue.push(m_blocks[e.target].node);
                    visited.insert(e.target);
                    changed = true;
                }
                else
                    LayeredLayout::remove_from_deque(
                        m_blocks[e.target].incoming, block.node);
            }
        }

        // No more nodes satisfy constraints, pick a node to continue
        // constructing the graph
        size_t best = 0, bestparent, bestedges;

        for(auto& item : m_blocks) {
            LLBlock& block = item.second;
            if(!visited.count(block.node))
                continue;

            const RDGraphEdge* edges = nullptr;
            size_t c = m_graph->outgoing(block.node, &edges);

            for(size_t i = 0; i < c; i++) {
                const RDGraphEdge& e = edges[i];
                if(visited.count(e.target))
                    continue;

                if(!best || (m_blocks[e.target].incoming.size() < bestedges) ||
                   ((m_blocks[e.target].incoming.size() == bestedges) &&
                    (e.target < best))) {
                    best = e.target;
                    bestedges =
                        static_cast<int>(m_blocks[e.target].incoming.size());
                    bestparent = block.node;
                }
            }
        }

        if(best) {
            LLBlock& bestparentb = m_blocks[bestparent];
            LayeredLayout::remove_from_deque(m_blocks[best].incoming,
                                             bestparentb.node);
            bestparentb.newoutgoing.push_back(best);
            visited.insert(best);
            queue.push(best);
            changed = true;
        }
    }
}

void LayeredLayout::prepare_edge_routing() {
    m_horizedges.resize(m_blocks[m_graph->root()].rowcount + 1);
    m_vertedges.resize(m_blocks[m_graph->root()].rowcount + 1);
    m_edgevalid.resize(m_blocks[m_graph->root()].rowcount + 1);

    for(int row = 0; row < m_blocks[m_graph->root()].rowcount + 1; row++) {
        m_horizedges[row].resize(m_blocks[m_graph->root()].colcount + 1);
        m_vertedges[row].resize(m_blocks[m_graph->root()].colcount + 1);
        LayeredLayout::init_deque(m_edgevalid[row],
                                  m_blocks[m_graph->root()].colcount + 1, true);

        for(int col = 0; col < m_blocks[m_graph->root()].colcount + 1; col++) {
            m_horizedges[row][col].clear();
            m_vertedges[row][col].clear();
        }
    }

    for(const auto& item : m_blocks) {
        const LLBlock& block = item.second;
        m_edgevalid[block.row][block.col + 1] = false;
    }
}

void LayeredLayout::perform_edge_routing() {
    for(const RDGraphNode& n : m_blockorder) {
        LLBlock& block = m_blocks[n];
        LLBlock& start = block;

        const RDGraphEdge* edges = nullptr;
        size_t c = m_graph->outgoing(block.node, &edges);

        for(size_t i = 0; i < c; i++) {
            const RDGraphEdge& e = edges[i];
            LLBlock& end = m_blocks[e.target];
            start.edges.push_back(this->route_edge(m_horizedges, m_vertedges,
                                                   m_edgevalid, start, end));
        }
    }
}

void LayeredLayout::compute_edge_count() {
    LayeredLayout::init_deque(m_coledgecount,
                              m_blocks[m_graph->root()].colcount + 1, 0);
    LayeredLayout::init_deque(m_rowedgecount,
                              m_blocks[m_graph->root()].rowcount + 1, 0);

    for(int row = 0; row < m_blocks[m_graph->root()].rowcount + 1; row++) {
        for(int col = 0; col < m_blocks[m_graph->root()].colcount + 1; col++) {
            if(static_cast<int>(m_horizedges[row][col].size()) >
               m_rowedgecount[row])
                m_rowedgecount[row] =
                    static_cast<int>(m_horizedges[row][col].size());

            if(static_cast<int>(m_vertedges[row][col].size()) >
               m_coledgecount[col])
                m_coledgecount[col] =
                    static_cast<int>(m_vertedges[row][col].size());
        }
    }
}

void LayeredLayout::compute_row_column_sizes() {
    LayeredLayout::init_deque(m_colwidth,
                              m_blocks[m_graph->root()].colcount + 1, 0);
    LayeredLayout::init_deque(m_rowheight,
                              m_blocks[m_graph->root()].rowcount + 1, 0);

    for(const auto& item : m_blocks) {
        const LLBlock& block = item.second;

        if((block.width / 2) > m_colwidth[block.col])
            m_colwidth[block.col] = block.width / 2;

        if((block.width / 2) > m_colwidth[block.col + 1])
            m_colwidth[block.col + 1] = block.width / 2;

        if(block.height > m_rowheight[block.row])
            m_rowheight[block.row] = block.height;
    }
}

void LayeredLayout::compute_row_column_positions() {
    LayeredLayout::init_deque(m_colx, m_blocks[m_graph->root()].colcount, 0);
    LayeredLayout::init_deque(m_rowy, m_blocks[m_graph->root()].rowcount, 0);
    LayeredLayout::init_deque(m_coledgex,
                              m_blocks[m_graph->root()].colcount + 1, 0);
    LayeredLayout::init_deque(m_rowedgey,
                              m_blocks[m_graph->root()].rowcount + 1, 0);

    int x = LLAYOUT_PADDING;

    for(int i = 0; i < m_blocks[m_graph->root()].colcount; i++) {
        m_coledgex[i] = x;
        x += LLAYOUT_PADDING_DIV2 * m_coledgecount[i];
        m_colx[i] = x;
        x += m_colwidth[i];
    }

    int y = LLAYOUT_PADDING;

    for(int i = 0; i < m_blocks[m_graph->root()].rowcount; i++) {
        m_rowedgey[i] = y;
        y += LLAYOUT_PADDING_DIV2 * m_rowedgecount[i];
        m_rowy[i] = y;
        y += m_rowheight[i];
    }

    m_coledgex[m_blocks[m_graph->root()].colcount] = x;
    m_rowedgey[m_blocks[m_graph->root()].rowcount] = y;

    m_graph->set_areawidth(
        x + LLAYOUT_PADDING +
        (LLAYOUT_PADDING_DIV2 *
         m_coledgecount[m_blocks[m_graph->root()].colcount]));

    m_graph->set_areaheight(
        y + LLAYOUT_PADDING +
        (LLAYOUT_PADDING_DIV2 *
         m_rowedgecount[m_blocks[m_graph->root()].rowcount]));
}

void LayeredLayout::compute_node_positions() {
    for(auto& item : m_blocks) {
        LLBlock& block = item.second;
        block.x = static_cast<float>(
            (m_colx[block.col] + m_colwidth[block.col] +
             (LLAYOUT_PADDING_DIV2 / 2.0) * m_coledgecount[block.col + 1]) -
            (block.width / 2.0));

        if((block.x + block.width) >
           (m_colx[block.col] + m_colwidth[block.col] +
            m_colwidth[block.col + 1] +
            LLAYOUT_PADDING_DIV2 * m_coledgecount[block.col + 1]))
            block.x = static_cast<float>(
                (m_colx[block.col] + m_colwidth[block.col] +
                 m_colwidth[block.col + 1] +
                 LLAYOUT_PADDING_DIV2 * m_coledgecount[block.col + 1]) -
                block.width);

        block.y = static_cast<float>(m_rowy[block.row] + LLAYOUT_PADDING);
        m_graph->set_x(block.node, static_cast<int>(block.x));
        m_graph->set_y(block.node, static_cast<int>(block.y));
    }
}

void LayeredLayout::precompute_edge_coordinates() {
    for(auto& item : m_blocks) {
        LLBlock& block = item.second;

        for(LLEdge& edge : block.edges) {
            const LLPoint& start = edge.points.front();
            auto startcol = start.col;
            auto lastindex = edge.startindex;
            RDGraphPoint lastpt = {
                m_coledgex[startcol] + (LLAYOUT_PADDING_DIV2 * lastindex) + 4,
                m_graph->y(block.node) + m_graph->height(block.node) + 4 -
                    LLAYOUT_NODE_PADDING};

            GraphPoints pts;
            pts.push_back(lastpt);

            for(const LLPoint& end : edge.points) {
                auto endrow = end.row;
                auto endcol = end.col;
                auto lastidx = end.index;
                RDGraphPoint newpt;

                if(startcol == endcol)
                    newpt = {lastpt.x, m_rowedgey[endrow] +
                                           (LLAYOUT_PADDING_DIV2 * lastidx) +
                                           4};
                else
                    newpt = {m_coledgex[endcol] +
                                 (LLAYOUT_PADDING_DIV2 * lastidx) + 4,
                             lastpt.y};

                pts.push_back(newpt);
                lastpt = newpt;
                startcol = endcol;
            }

            RDGraphPoint newpt = {lastpt.x,
                                  m_graph->y(edge.targetblock->node) - 1};
            pts.push_back(newpt);
            edge.routes = pts;

            pts = {};
            pts.push_back({newpt.x - 3, newpt.y - 6});
            pts.push_back({newpt.x + 3, newpt.y - 6});
            pts.push_back(newpt);
            edge.arrow = pts;

            const RDGraphEdge* e =
                m_graph->edge(edge.sourceblock->node, edge.targetblock->node);
            if(!e)
                continue;

            m_graph->set_routes(e, edge.routes.data(), edge.routes.size());
            m_graph->set_arrow(e, edge.arrow.data(), edge.arrow.size());
        }
    }
}

LLEdge LayeredLayout::route_edge(LayeredLayout::EdgesVector& horizedges,
                                 LayeredLayout::EdgesVector& vertedges,
                                 Matrix<bool>& edgevalid, LLBlock& start,
                                 LLBlock& end) {
    LLEdge edge;
    edge.sourceblock = &start;
    edge.targetblock = &end;

    // Find edge index for initial outgoing line
    int i = 0;

    while(true) {
        if(!this->is_edge_marked(vertedges, start.row + 1, start.col + 1, i))
            break;
        i += 1;
    }

    this->mark_edge(vertedges, start.row + 1, start.col + 1, i);
    edge.add_point(start.row + 1, start.col + 1);
    edge.startindex = i;
    bool horiz = false;

    // Find valid column for moving vertically to the target node
    int minrow, maxrow;

    if(end.row < (start.row + 1)) {
        minrow = end.row;
        maxrow = start.row + 1;
    }
    else {
        minrow = start.row + 1;
        maxrow = end.row;
    }

    int col = start.col + 1;

    if(minrow != maxrow) {
        auto check_column = [minrow, maxrow, &edgevalid](int column) -> bool {
            if(column < 0 ||
               column >= static_cast<int>(edgevalid[minrow].size()))
                return false;

            for(int row = minrow; row < maxrow; row++) {
                if(!edgevalid[row][column])
                    return false;
            }

            return true;
        };

        if(!check_column(col)) {
            if(!check_column(end.col + 1)) {
                int ofs = 0;

                while(true) {
                    col = start.col + 1 - ofs;
                    if(check_column(col))
                        break;

                    col = start.col + 1 + ofs;
                    if(check_column(col))
                        break;
                    ofs += 1;
                }
            }
            else
                col = end.col + 1;
        }
    }

    if(col != (start.col + 1)) {
        // Not in same column, need to generate a line for moving to the correct
        // column
        int mincol, maxcol;

        if(col < (start.col + 1)) {
            mincol = col;
            maxcol = start.col + 1;
        }
        else {
            mincol = start.col + 1;
            maxcol = col;
        }

        int index = this->find_horiz_edge_index(horizedges, start.row + 1,
                                                mincol, maxcol);
        edge.add_point(start.row + 1, col, index);
        horiz = true;
    }

    if(end.row != (start.row + 1)) {
        // Not in same row, need to generate a line for moving to the correct
        // row
        if(col == (start.col + 1))
            this->mark_edge(vertedges, start.row + 1, start.col + 1, i, false);
        int index = this->find_vert_edge_index(vertedges, col, minrow, maxrow);
        if(col == (start.col + 1))
            edge.startindex = index;
        edge.add_point(end.row, col, index);
        horiz = false;
    }

    if(col != (end.col + 1)) {
        // Not in ending column, need to generate a line for moving to the
        // correct column
        int mincol, maxcol;
        if(col < (end.col + 1)) {
            mincol = col;
            maxcol = end.col + 1;
        }
        else {
            mincol = end.col + 1;
            maxcol = col;
        }
        int index =
            this->find_horiz_edge_index(horizedges, end.row, mincol, maxcol);
        edge.add_point(end.row, end.col + 1, index);
        horiz = true;
    }

    // If last line was horizontal, choose the ending edge index for the
    // incoming edge
    if(horiz) {
        int index = this->find_vert_edge_index(vertedges, end.col + 1, end.row,
                                               end.row);
        edge.points[static_cast<int>(edge.points.size()) - 1].index = index;
    }

    return edge;
}

int LayeredLayout::find_horiz_edge_index(LayeredLayout::EdgesVector& edges,
                                         int row, int mincol,
                                         int maxcol) const {
    // Find a valid index
    int i = 0;

    while(true) {
        bool valid = true;

        for(int col = mincol; col < maxcol + 1; col++) {
            if(!this->is_edge_marked(edges, row, col, i))
                continue;
            valid = false;
            break;
        }

        if(valid)
            break;
        i++;
    }

    // Mark chosen index as used
    for(int col = mincol; col < maxcol + 1; col++)
        this->mark_edge(edges, row, col, i);
    return i;
}

int LayeredLayout::find_vert_edge_index(LayeredLayout::EdgesVector& edges,
                                        int col, int minrow, int maxrow) const {
    // Find a valid index
    int i = 0;

    while(true) {
        bool valid = true;

        for(int row = minrow; row < maxrow + 1; row++) {
            if(!this->is_edge_marked(edges, row, col, i))
                continue;
            valid = false;
            break;
        }

        if(valid)
            break;
        i++;
    }

    // Mark chosen index as used
    for(int row = minrow; row < maxrow + 1; row++)
        this->mark_edge(edges, row, col, i);
    return i;
}

bool LayeredLayout::is_edge_marked(LayeredLayout::EdgesVector& edges, int row,
                                   int col, int index) const {
    if(index >= static_cast<int>(edges[row][col].size()))
        return false;
    return edges[row][col][index];
}

void LayeredLayout::mark_edge(LayeredLayout::EdgesVector& edges, int row,
                              int col, int index, bool used) const {
    while(static_cast<int>(edges[row][col].size()) <= index)
        edges[row][col].push_back(false);

    edges[row][col][index] = used;
}

void LayeredLayout::adjust_graph_layout(LLBlock& block, int col, int row) {
    block.col += col;
    block.row += row;

    for(const RDGraphNode& n : block.newoutgoing)
        this->adjust_graph_layout(m_blocks[n], col, row);
}

void LayeredLayout::compute_layout(LLBlock& block) {
    // Compute child node layouts and arrange them horizontally
    int col = 0;
    int rowcount = 1;
    int childcolumn = 0;
    bool singlechild = block.newoutgoing.size() == 1;

    for(RDGraphNode n : block.newoutgoing) {
        this->compute_layout(m_blocks[n]);

        if((m_blocks[n].rowcount + 1) > rowcount)
            rowcount = m_blocks[n].rowcount + 1;

        childcolumn = m_blocks[n].col;
    }

    if(m_layouttype != LAYEREDLAYOUT_WIDE && block.newoutgoing.size() == 2) {
        LLBlock& left = m_blocks[block.newoutgoing[0]];
        LLBlock& right = m_blocks[block.newoutgoing[1]];

        if(left.newoutgoing.size() == 0) {
            left.col = right.col - 2;
            int add = left.col < 0 ? -left.col : 0;
            this->adjust_graph_layout(right, add, 1);
            this->adjust_graph_layout(left, add, 1);
            col = right.colcount + add;
        }
        else if(right.newoutgoing.size() == 0) {
            this->adjust_graph_layout(left, 0, 1);
            this->adjust_graph_layout(right, left.col + 2, 1);
            col = std::max(left.colcount, right.col + 2);
        }
        else {
            this->adjust_graph_layout(left, 0, 1);
            this->adjust_graph_layout(right, left.colcount, 1);
            col = left.colcount + right.colcount;
        }

        block.colcount = std::max(2, col);

        if(m_layouttype == LAYEREDLAYOUT_MEDIUM)
            block.col = (left.col + right.col) / 2;
        else
            block.col = singlechild ? childcolumn : (col - 2) / 2;
    }
    else {
        for(const RDGraphNode& n : block.newoutgoing) {
            this->adjust_graph_layout(m_blocks[n], col, 1);
            col += m_blocks[n].colcount;
        }

        if(col >= 2) {
            // Place this node centered over the child nodes
            block.col = singlechild ? childcolumn : (col - 2) / 2;
            block.colcount = col;
        }
        else {
            // No child nodes, set single node's width (nodes are 2 columns wide
            // to allow centering over a branch)
            block.col = 0;
            block.colcount = 2;
        }
    }

    block.row = 0;
    block.rowcount = rowcount;
}

} // namespace redasm
