#include "surfacegraph.h"
#include "surfacegraphitem.h"

SurfaceGraph::SurfaceGraph(QWidget* parent): GraphView{parent} {
    m_surface = rdsurface_new(SURFACE_DEFAULT);
}

SurfaceGraph::~SurfaceGraph() {
    rd_free(m_surface);
    m_surface = nullptr;
}

void SurfaceGraph::jump_to(RDAddress address) {}

void SurfaceGraph::set_location(const RDSurfaceLocation& loc) {
    if(!loc.function.valid)
        m_function = nullptr;
    else
        m_function = rd_getfunction(loc.function.value);

    m_graph = m_function ? rdfunction_getgraph(m_function) : nullptr;
    this->set_graph(m_graph);
    this->update_graph();
}

void SurfaceGraph::invalidate() {}

GraphViewItem* SurfaceGraph::create_item(RDGraphNode n, const RDGraph*) {
    RDFunctionBasicBlock bb;

    if(rdfunction_getbasicblock(m_function, n, &bb))
        return new SurfaceGraphItem(m_surface, bb, n, m_function, this);

    return nullptr;
}

void SurfaceGraph::keyPressEvent(QKeyEvent* e) {
    if(e->key() == Qt::Key_Space)
        Q_EMIT switch_view();
    else
        GraphView::keyPressEvent(e);
}
