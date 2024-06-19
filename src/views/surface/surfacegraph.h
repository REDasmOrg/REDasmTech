#pragma once

#include "../graphview/graphview.h"

class SurfaceGraph: public GraphView {
    Q_OBJECT

public:
    explicit SurfaceGraph(QWidget* parent = nullptr);
    ~SurfaceGraph() override;
    void jump_to(RDAddress address);
    void set_location(const RDSurfaceLocation& loc);
    void invalidate();

protected:
    GraphViewItem* create_item(RDGraphNode n, const RDGraph* g) override;
    void keyPressEvent(QKeyEvent* e) override;

Q_SIGNALS:
    void switch_view();

private:
    RDFunction* m_function{nullptr};
    RDSurface* m_surface{nullptr};
};
