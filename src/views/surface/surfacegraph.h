#pragma once

#include "../graphview/graphview.h"
#include <QMenu>

class SurfaceGraph: public GraphView {
    Q_OBJECT

public:
    explicit SurfaceGraph(QWidget* parent = nullptr);
    ~SurfaceGraph() override;
    [[nodiscard]] inline RDSurface* handle() const { return m_surface; }
    RDSurfaceLocation location() const;
    void jump_to(RDAddress address);
    void set_location(const RDSurfaceLocation& loc);
    void invalidate();

protected:
    void begin_compute() override;
    void end_compute() override;
    void update_edge(const RDGraphEdge& e) override;
    void update_node(GraphViewItem*) override;
    GraphViewItem* create_node(RDGraphNode n, const RDGraph* g) override;
    void keyPressEvent(QKeyEvent* e) override;

Q_SIGNALS:
    void switch_view();

private:
    RDFunction* m_function{nullptr};
    RDSurface* m_surface{nullptr};
    QMenu* m_menu;
};
