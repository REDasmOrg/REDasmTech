#include "surfacegraph.h"

SurfaceGraph::SurfaceGraph(QWidget* parent): GraphView{parent} {}

GraphViewItem* SurfaceGraph::create_item(RDGraphNode n, const RDGraph* g) {
    return nullptr;
}

void SurfaceGraph::keyPressEvent(QKeyEvent* e) {
    if(e->key() == Qt::Key_Space)
        Q_EMIT switch_view();
    else
        GraphView::keyPressEvent(e);
}
