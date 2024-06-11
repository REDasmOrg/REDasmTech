#pragma once

#include "../graphview/graphview.h"

class SurfaceGraph: public GraphView {
    Q_OBJECT

public:
    explicit SurfaceGraph(QWidget* parent = nullptr);

protected:
    GraphViewItem* create_item(RDGraphNode n, const RDGraph* g) override;
    void keyPressEvent(QKeyEvent* e) override;

Q_SIGNALS:
    void switch_view();
};
