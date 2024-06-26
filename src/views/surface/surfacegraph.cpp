#include "surfacegraph.h"
#include "../../themeprovider.h"
#include "surfacegraphitem.h"

SurfaceGraph::SurfaceGraph(QWidget* parent): GraphView{parent} {
    m_surface = rdsurface_new(SURFACE_GRAPH);
}

SurfaceGraph::~SurfaceGraph() {
    rd_free(m_surface);
    m_surface = nullptr;
}

void SurfaceGraph::jump_to(RDAddress address) {}

void SurfaceGraph::set_location(const RDSurfaceLocation& loc) {
    if(loc.function.valid)
        m_function = rd_getfunction(loc.function.value);
    else
        m_function = nullptr;

    if(m_function)
        m_graph = rdfunction_getgraph(m_function);
    else
        m_graph = nullptr;

    this->set_graph(m_graph);
}

void SurfaceGraph::invalidate() {}

void SurfaceGraph::begin_compute() {
    if(m_surface && m_function)
        rdsurface_renderfunction(m_surface, m_function);
}

void SurfaceGraph::update_edge(const RDGraphEdge& e) {
    RDThemeKind theme = rdfunction_gettheme(m_function, &e);
    rdgraph_setcolor(m_graph, &e,
                     qUtf8Printable(themeprovider::color(theme).name()));
}

void SurfaceGraph::update_node(GraphViewItem* item) {
    auto* g = static_cast<SurfaceGraphItem*>(item);
    g->update_document();
}

GraphViewItem* SurfaceGraph::create_node(RDGraphNode n, const RDGraph*) {
    RDFunctionBasicBlock bb;

    if(rdfunction_getbasicblock(m_function, n, &bb)) {
        auto* g = new SurfaceGraphItem(m_surface, bb, n, m_function, this);
        g->setObjectName(QString::number(bb.start, 16));
        connect(g, &SurfaceGraphItem::invalidated, this,
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
