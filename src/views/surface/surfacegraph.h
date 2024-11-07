#pragma once

#include "../graphview/graphview.h"
#include "surfacegraphnode.h"
#include <QMenu>

class SurfaceGraph: public GraphView {
    Q_OBJECT

public:
    explicit SurfaceGraph(QWidget* parent = nullptr);
    ~SurfaceGraph() override;
    [[nodiscard]] RDSurface* handle() const { return m_surface; }
    [[nodiscard]] bool can_goback() const;
    [[nodiscard]] bool can_goforward() const;
    [[nodiscard]] bool has_rdil() const;
    RDSurfaceLocation location() const;
    void jump_to(RDAddress address);
    void invalidate();
    void set_rdil(bool b);

    void set_location(const RDSurfaceLocation& loc) {
        this->set_location(loc, true);
        Q_EMIT history_updated();
    }

public Q_SLOTS:
    bool go_back();
    bool go_forward();
    void clear_history();

protected:
    void begin_compute() override;
    void end_compute() override;
    void update_edge(const RDGraphEdge& e) override;
    void update_node(GraphViewNode*) override;
    GraphViewNode* create_node(RDGraphNode n, const RDGraph* g) override;
    void keyPressEvent(QKeyEvent* e) override;

private:
    SurfaceGraphNode* find_node(RDAddress address);
    bool seek_to_address(RDAddress address);
    void set_location(const RDSurfaceLocation& loc, bool seek);

Q_SIGNALS:
    void history_updated();
    void switch_view();

private:
    RDFunction* m_function{nullptr};
    RDSurface* m_surface{nullptr};
    QMenu* m_menu;
};
