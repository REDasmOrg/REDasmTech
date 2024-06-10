#pragma once

// Widget based on x64dbg's DisassemblerGraphView
//   https://github.com/x64dbg/x64dbg/blob/development/src/gui/Src/Gui/DisassemblerGraphView.h
//   https://github.com/x64dbg/x64dbg/blob/development/src/gui/Src/Gui/DisassemblerGraphView.cpp

#include "graphviewitem.h"
#include <QAbstractScrollArea>
#include <QList>
#include <QVector>
#include <redasm/redasm.h>
#include <unordered_map>

class GraphView: public QAbstractScrollArea {
    Q_OBJECT

public:
    explicit GraphView(QWidget* parent = nullptr);
    void set_graph(RDGraph* graph);
    void set_selected_block(GraphViewItem* item);
    void set_focus_on_selection(bool b);
    void update_graph();
    GraphViewItem* selected_item() const;
    RDGraph* graph() const;

public Q_SLOTS:
    void focus_selected_block();

protected:
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* e) override;
    void paintEvent(QPaintEvent*) override;
    void showEvent(QShowEvent* e) override;
    void focus_block(const GraphViewItem* item, bool force = false);
    virtual void selected_item_changed_event();
    virtual GraphViewItem* create_item(RDGraphNode n, const RDGraph* g) = 0;
    virtual void compute_edge(const RDGraphEdge&);
    virtual void compute_node(GraphViewItem*);
    virtual void compute_layout();
    virtual void computed();

private:
    GraphViewItem* item_from_mouse_event(QMouseEvent* e,
                                         QPoint* itempos = nullptr) const;
    void zoom_out(const QPointF& cursorpos);
    void zoom_in(const QPointF& cursorpos);
    void adjustSize(int vpw, int vph, const QPointF& cursorpos = QPoint(),
                    bool fit = false);
    void precompute_arrow(const RDGraphEdge& e);
    void precompute_line(const RDGraphEdge& e);
    bool update_selected_item(QMouseEvent* e, QPoint* itempos = nullptr);

Q_SIGNALS:
    void selected_item_changed();

protected:
    RDGraph* m_graph{nullptr};
    QHash<RDGraphNode, GraphViewItem*> m_items;

private:
    GraphViewItem* m_selecteditem{nullptr};
    std::unordered_map<RDGraphEdge, QVector<QLine>> m_lines;
    std::unordered_map<RDGraphEdge, QPolygon> m_arrows;
    QPoint m_renderoffset, m_scrollbase;
    QSize m_rendersize;
    qreal m_scalefactor{1.0}, m_scalestep{0.1}, m_prevscalefactor{0};
    qreal m_scalemin{0}, m_scalemax{5.0};
    int m_scaledirection{0}, m_scaleboost{1};
    bool m_viewportready{false}, m_scrollmode{true};
    bool m_focusonselection{false};
};
