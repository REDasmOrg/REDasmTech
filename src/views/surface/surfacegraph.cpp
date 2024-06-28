#include "surfacegraph.h"
#include "../../statusbar.h"
#include "../../themeprovider.h"
#include "../../utils.h"

SurfaceGraph::SurfaceGraph(QWidget* parent): GraphView{parent} {
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    m_surface = rdsurface_new(SURFACE_GRAPH);
    m_menu = utils::create_surface_menu(m_surface, this);

    connect(this, &SurfaceGraph::customContextMenuRequested, this,
            [&](const QPoint&) {
                if(this->selected_item())
                    m_menu->popup(QCursor::pos());
            });
}

SurfaceGraph::~SurfaceGraph() {
    rd_free(m_surface);
    m_surface = nullptr;
}

RDSurfaceLocation SurfaceGraph::location() const {
    RDSurfaceLocation loc;
    rdsurface_getlocation(m_surface, &loc);
    return loc;
}

void SurfaceGraph::jump_to(RDAddress address) {
    if(this->seek_to_address(address))
        this->set_location(this->location(), false);
}

void SurfaceGraph::invalidate() { this->update_graph(); }

void SurfaceGraph::begin_compute() {
    if(m_surface && m_function)
        rdsurface_renderfunction(m_surface, m_function);
}

void SurfaceGraph::end_compute() { statusbar::set_location(m_surface); }

void SurfaceGraph::update_edge(const RDGraphEdge& e) {
    RDThemeKind theme = rdfunction_gettheme(m_function, &e);
    rdgraph_setcolor(m_graph, &e,
                     qUtf8Printable(themeprovider::color(theme).name()));
}

void SurfaceGraph::update_node(GraphViewNode* item) {
    auto* g = static_cast<SurfaceGraphNode*>(item);
    g->update_document();
}

GraphViewNode* SurfaceGraph::create_node(RDGraphNode n, const RDGraph*) {
    RDFunctionBasicBlock bb;

    if(rdfunction_getbasicblock(m_function, n, &bb)) {
        auto* g = new SurfaceGraphNode(m_surface, bb, n, m_function, this);
        g->setObjectName(QString::number(bb.start, 16));

        connect(g, &SurfaceGraphNode::follow_requested, this, [&]() {
            RDAddress address;

            if(rdsurface_getaddressundercursor(m_surface, &address))
                this->jump_to(address);
        });

        connect(g, &SurfaceGraphNode::invalidated, this,
                &SurfaceGraph::update_graph);
        return g;
    }

    return nullptr;
}

void SurfaceGraph::keyPressEvent(QKeyEvent* e) {
    if(e->key() == Qt::Key_Space)
        Q_EMIT switch_view();
    else
        GraphView::keyPressEvent(e);
}

SurfaceGraphNode* SurfaceGraph::find_node(RDAddress address) {
    for(GraphViewNode* g : m_nodes) {
        auto* sg = static_cast<SurfaceGraphNode*>(g);
        if(sg->contains_address(address))
            return sg;
    }

    return nullptr;
}

bool SurfaceGraph::seek_to_address(RDAddress address) {
    LIndex idx;

    if(!rdlisting_getindex(address, &idx))
        return false;

    rdsurface_seekposition(m_surface, idx);
    return true;
}

void SurfaceGraph::set_location(const RDSurfaceLocation& loc, bool seek) {
    if(loc.function.valid)
        m_function = rd_getfunction(loc.function.value);
    else
        m_function = nullptr;

    if(m_function)
        m_graph = rdfunction_getgraph(m_function);
    else
        m_graph = nullptr;

    if(seek && loc.address.valid)
        this->seek_to_address(loc.address.value);

    this->set_graph(m_graph);

    if(m_graph) {
        if(!loc.address.valid)
            return;

        if(SurfaceGraphNode* n = this->find_node(loc.address.value); n)
            this->set_selected_node(n);
    }
    else // No graph, switch to listing
        Q_EMIT switch_view();
}
