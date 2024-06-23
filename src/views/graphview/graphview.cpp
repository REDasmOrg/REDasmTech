#include "graphview.h"
#include "../../themeprovider.h"
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QWheelEvent>
#include <cmath>
#include <redasm/redasm.h>

GraphView::GraphView(QWidget* parent): QAbstractScrollArea(parent) {
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, themeprovider::color(THEME_GRAPHBG));

    this->horizontalScrollBar()->setSingleStep(this->fontMetrics().height());
    this->verticalScrollBar()->setSingleStep(this->fontMetrics().height());
    this->setFrameShape(QFrame::NoFrame);
    this->setAutoFillBackground(true);
    this->setPalette(palette);
}

void GraphView::set_graph(RDGraph* graph) {
    m_scalefactor = m_scaleboost = 1.0;
    m_graph = graph;
    this->update_graph();
}

void GraphView::set_selected_block(GraphViewItem* item) {
    for(GraphViewItem* gvi : m_items) {
        if(gvi != item)
            continue;
        m_selecteditem = item;
        this->focus_selected_block();
        break;
    }
}

void GraphView::set_focus_on_selection(bool b) { m_focusonselection = b; }

GraphViewItem* GraphView::selected_item() const { return m_selecteditem; }
RDGraph* GraphView::graph() const { return m_graph; }

void GraphView::focus_selected_block() {
    if(m_selecteditem)
        this->focus_block(m_selecteditem);
}

void GraphView::focus_block(const GraphViewItem* item, bool force) {
    // Don't update the view for blocks that are already fully in view
    int xofs = this->horizontalScrollBar()->value();
    int yofs = this->verticalScrollBar()->value();
    QRect viewportrect = this->viewport()->rect();

    // before being shown for the first time viewport is kind of 98x28 so
    // setting the parent size almost fixes this problem
    if(!m_viewportready)
        viewportrect.setSize(this->parentWidget()->size() - QSize(20, 20));

    QPoint translation(m_renderoffset.x() - xofs, m_renderoffset.y() - yofs);

    // Adjust scaled viewport
    viewportrect.setWidth(viewportrect.width() / m_scalefactor);
    viewportrect.setHeight(viewportrect.height() / m_scalefactor);
    viewportrect.translate(-translation.x() / m_scalefactor,
                           -translation.y() / m_scalefactor);

    QFontMetrics fm = this->fontMetrics();
    QRect r(item->x() + fm.height(), item->width() + fm.height(),
            item->width() - (2 * fm.height()),
            item->height() - (2 * fm.height()));

    if(force || !viewportrect.contains(r)) {
        auto x =
            (item->x() + static_cast<int>(item->width() / 2)) * m_scalefactor;
        auto y = (item->y() + (2 * fm.height()) +
                  (item->current_row() * fm.height())) *
                 m_scalefactor;
        this->horizontalScrollBar()->setValue(
            x + m_renderoffset.x() -
            static_cast<int>(this->horizontalScrollBar()->pageStep() / 2));
        this->verticalScrollBar()->setValue(
            y + m_renderoffset.y() -
            static_cast<int>(this->verticalScrollBar()->pageStep() / 2));
    }
}

void GraphView::mouseDoubleClickEvent(QMouseEvent* e) {
    QPoint itempos;
    bool updated = this->update_selected_item(e, &itempos);

    if(m_selecteditem && (e->buttons() == Qt::LeftButton)) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QMouseEvent iteme{e->type(),   itempos,      e->globalPosition(),
                          e->button(), e->buttons(), e->modifiers()};
#else
        QMouseEvent iteme{e->type(),   itempos,      e->globalPos(),
                          e->button(), e->buttons(), e->modifiers()};
#endif
        m_selecteditem->mousedoubleclick_event(&iteme);
    }

    if(updated)
        this->selected_item_changed_event();
    QAbstractScrollArea::mouseDoubleClickEvent(e);
}

void GraphView::mousePressEvent(QMouseEvent* e) {
    QPoint itempos;
    bool updated = this->update_selected_item(e, &itempos);

    if(m_selecteditem) {
        QMouseEvent iteme{
            e->type(),   itempos,      e->globalPosition(),
            e->button(), e->buttons(), e->modifiers(),
        };
        m_selecteditem->mousepress_event(&iteme);
    }
    else if(e->button() == Qt::LeftButton) {
        m_scrollmode = true;
        m_scrollbase = e->pos();
        this->setCursor(Qt::ClosedHandCursor);
        this->viewport()->grabMouse();
    }

    this->viewport()->update();
    if(updated)
        this->selected_item_changed_event();
    QAbstractScrollArea::mousePressEvent(e);
}

void GraphView::mouseReleaseEvent(QMouseEvent* e) {
    this->viewport()->update();

    if(e->button() == Qt::LeftButton && m_scrollmode) {
        m_scrollmode = false;
        this->setCursor(Qt::ArrowCursor);
        this->viewport()->releaseMouse();
    }

    QAbstractScrollArea::mouseReleaseEvent(e);
}

void GraphView::mouseMoveEvent(QMouseEvent* e) {
    if(m_selecteditem) {
        QPoint itempos;
        GraphViewItem* item = this->item_from_mouse_event(e, &itempos);

        if(item == m_selecteditem) {
            QMouseEvent iteme{
                e->type(),   itempos,      e->globalPosition(),
                e->button(), e->buttons(), e->modifiers(),
            };
            m_selecteditem->mousemove_event(&iteme);
        }

        return;
    }

    if(m_scrollmode) {

        QPoint delta{
            m_scrollbase.x() - qRound(e->position().x()),
            m_scrollbase.y() - qRound(e->position().y()),
        };

        m_scrollbase = e->pos();

        this->horizontalScrollBar()->setValue(
            this->horizontalScrollBar()->value() + delta.x());
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() +
                                            delta.y());
    }

    QAbstractScrollArea::mouseMoveEvent(e);
}

void GraphView::wheelEvent(QWheelEvent* event) {
    if(event->modifiers() & Qt::ControlModifier) {
        m_scaleboost = event->modifiers() & Qt::ShiftModifier ? 2 : 1;

        QPoint ndegrees = event->angleDelta() / 8;
        QPoint nsteps = ndegrees / 15;

        if(nsteps.y() > 0) {
            m_scaledirection = 1;
            this->zoom_in(event->position());
        }
        else if(nsteps.y() < 0) {
            m_scaledirection = -1;
            this->zoom_out(event->position());
        }

        event->accept();
        return;
    }

    QAbstractScrollArea::wheelEvent(event);
}

void GraphView::resizeEvent(QResizeEvent* e) {
    this->adjustSize(e->size().width(), e->size().height());
}

void GraphView::paintEvent(QPaintEvent*) {
    QPoint translation = {
        m_renderoffset.x() - this->horizontalScrollBar()->value(),
        m_renderoffset.y() - this->verticalScrollBar()->value()};

    QPainter painter(this->viewport());
    painter.translate(translation);
    painter.scale(m_scalefactor, m_scalefactor);

    QRect vpr(this->viewport()->rect().topLeft(),
              this->viewport()->rect().bottomRight() - QPoint(1, 1));

    // Adjust imaginary viewport rect at new zoom level
    vpr.setWidth(vpr.width() / m_scalefactor);
    vpr.setHeight(vpr.height() / m_scalefactor);
    vpr.translate(-translation.x() / m_scalefactor,
                  -translation.y() / m_scalefactor);

    // Render edges
    painter.save();

    for(auto it = m_lines.begin(); it != m_lines.end(); it++) {
        QColor c(rdgraph_getcolor(m_graph, std::addressof(it->first)));
        QPen pen(c);

        if(m_selecteditem && ((it->first.src == m_selecteditem->node()) ||
                              (it->first.src == m_selecteditem->node())))
            pen.setWidthF(2.0 / m_scalefactor);
        else {
            pen.setWidthF(1.0 / m_scalefactor);
            pen.setStyle(m_selecteditem ? Qt::DashLine : Qt::SolidLine);
        }

        painter.setPen(pen);
        painter.setBrush(c);
        painter.drawLines(it->second);

        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);
        painter.drawConvexPolygon(m_arrows[it->first]);
    }

    painter.restore();

    // Render nodes
    for(auto* item : m_items) {
        if(!vpr.intersects(item->rect())) // Ignore blocks that are not in view
            continue;

        usize itemstate = GraphViewItem::NONE;
        if(m_selecteditem == item)
            itemstate |= GraphViewItem::SELECTED;

        item->render(&painter, itemstate);
    }
}

void GraphView::showEvent(QShowEvent* e) {
    if(!m_viewportready)
        m_viewportready = true;
    e->ignore();
}

void GraphView::selected_item_changed_event() {
    if(m_focusonselection)
        this->focus_selected_block();
    Q_EMIT selected_item_changed();
}

void GraphView::compute_edge(const RDGraphEdge&) {}
void GraphView::compute_node(GraphViewItem*) {}

void GraphView::computed() {
    auto it = m_items.find(rdgraph_getroot(m_graph));
    if(it != m_items.end())
        this->focus_block(it.value());
}

void GraphView::compute_layout() {
    rdgraphlayout_layered(m_graph, LAYEREDLAYOUT_MEDIUM);
}

void GraphView::update_graph() {
    m_selecteditem = nullptr;
    qDeleteAll(m_items);
    m_items.clear();
    m_lines.clear();
    m_arrows.clear();

    if(!m_graph) {
        this->viewport()->update();
        return;
    }

    rdgraph_clearlayout(m_graph);

    RDGraphNode root = rdgraph_getroot(m_graph);

    const RDGraphNode* nodes = nullptr;
    usize nc = rdgraph_getnodes(m_graph, &nodes);

    for(usize i = 0; i < nc; i++) {
        RDGraphNode n = nodes[i];
        auto* item = this->create_item(n, m_graph);
        if(!item)
            continue;
        if(n == root)
            m_selecteditem = item; // Focus on root

        item->setParent(this); // Take Ownership
        m_items[n] = item;
        this->compute_node(item);

        rdgraph_setwidth(m_graph, item->node(), item->width());
        rdgraph_setheight(m_graph, item->node(), item->height());
    }

    const RDGraphEdge* edges = nullptr;
    usize ec = rdgraph_getedges(m_graph, &edges);
    for(usize i = 0; i < ec; i++)
        this->compute_edge(edges[i]);

    this->compute_layout();

    for(usize i = 0; i < nc; i++) {
        RDGraphNode n = nodes[i];
        m_items[n]->move(
            QPoint(rdgraph_getx(m_graph, n), rdgraph_gety(m_graph, n)));
        connect(m_items[n], &GraphViewItem::invalidated, this->viewport(),
                [&]() { this->viewport()->update(); });
    }

    for(usize i = 0; i < ec; i++) {
        const RDGraphEdge& e = edges[i];
        this->precompute_line(e);
        this->precompute_arrow(e);
    }

    QSize areasize;
    if(m_viewportready)
        areasize = this->viewport()->size();
    else
        areasize = this->parentWidget()->size() - QSize(20, 20);

    qreal sx = static_cast<qreal>(areasize.width()) /
               static_cast<qreal>(this->width());
    qreal sy = static_cast<qreal>(areasize.height()) /
               static_cast<qreal>(this->height());
    m_scalemin =
        std::min(static_cast<qreal>(std::min(sx, sy) * (1 - m_scalestep)),
                 0.05); // If graph is very large...

    this->adjustSize(areasize.width(), areasize.height());
    this->viewport()->update();
    this->computed();
}

GraphViewItem* GraphView::item_from_mouse_event(QMouseEvent* e,
                                                QPoint* itempos) const {
    // Convert coordinates to system used in blocks
    int xofs = this->horizontalScrollBar()->value();
    int yofs = this->verticalScrollBar()->value();

    QPoint pos = {
        static_cast<int>(std::floor(
            (e->position().x() + xofs - m_renderoffset.x()) / m_scalefactor)),
        static_cast<int>(std::floor(
            (e->position().y() + yofs - m_renderoffset.y()) / m_scalefactor)),
    };

    for(GraphViewItem* item : m_items) {
        if(!item->contains(pos))
            continue;
        if(itempos)
            *itempos = item->map_to_item(pos);
        return item;
    }

    return nullptr;
}

void GraphView::zoom_out(const QPointF& cursorpos) {
    m_prevscalefactor = m_scalefactor;

    if(m_scalefactor <= m_scalemin)
        return;
    m_scalefactor *= (1 - m_scalestep * m_scaleboost);

    if(m_scalefactor < m_scalemin)
        m_scalefactor = m_scalemin;

    QSize vpsize = this->viewport()->size();
    this->adjustSize(vpsize.width(), vpsize.height(), cursorpos);
    this->viewport()->update();
}

void GraphView::zoom_in(const QPointF& cursorpos) {
    m_prevscalefactor = m_scalefactor;

    if(m_scalefactor >= m_scalemax)
        return;
    m_scalefactor /= (1 - m_scalestep * m_scaleboost);

    if(m_scalefactor > m_scalemax)
        m_scalefactor = m_scalemax;

    QSize vpsize = this->viewport()->size();
    this->adjustSize(vpsize.width(), vpsize.height(), cursorpos);
    this->viewport()->update();
}

void GraphView::adjustSize(int vpw, int vph, const QPointF& cursorpos,
                           bool fit) {
    // bugfix - resize event (during several initial calls) may reset correct
    // adjustment already made
    if(!m_graph || (vph < 30))
        return;

    m_rendersize = QSize{
        static_cast<int>(rdgraph_getareawidth(m_graph) * m_scalefactor),
        static_cast<int>(rdgraph_getareaheight(m_graph) * m_scalefactor),
    };

    m_renderoffset = QPoint(vpw, vph);

    QSize scrollrange = {
        m_rendersize.width() + vpw,
        m_rendersize.height() + vph,
    };

    qreal scalestepreal = 0.0;

    if(m_scaledirection > 0) {
        scalestepreal = (m_scalefactor - m_prevscalefactor) / m_scalefactor;
        scalestepreal /= (1 - scalestepreal);
    }
    else
        scalestepreal = (m_prevscalefactor - m_scalefactor) / m_prevscalefactor;

    QPoint deltaoffset{
        static_cast<int>(m_renderoffset.x() * scalestepreal * m_scaledirection),
        static_cast<int>(m_renderoffset.y() * scalestepreal * m_scaledirection),
    };

    QPoint oldscrollpos{
        this->horizontalScrollBar()->value(),
        this->verticalScrollBar()->value(),
    };

    this->horizontalScrollBar()->setPageStep(vpw);
    this->horizontalScrollBar()->setRange(0, scrollrange.width());
    this->verticalScrollBar()->setPageStep(vph);
    this->verticalScrollBar()->setRange(0, scrollrange.height());

    if(!cursorpos.isNull()) {
        QPointF deltacursorreal =
            cursorpos / m_prevscalefactor + oldscrollpos / m_prevscalefactor;
        QPointF deltacursordiff = deltacursorreal * m_scalefactor -
                                  deltacursorreal * m_prevscalefactor;

        this->horizontalScrollBar()->setValue(
            qRound(oldscrollpos.x() + deltacursordiff.x() - deltaoffset.x()));
        this->verticalScrollBar()->setValue(
            qRound(oldscrollpos.y() + deltacursordiff.y() - deltaoffset.y()));
    }
    else if(fit) {
        this->horizontalScrollBar()->setValue(scrollrange.width() / 2);
        this->verticalScrollBar()->setValue(scrollrange.height() / 2);
    }
}

void GraphView::precompute_arrow(const RDGraphEdge& e) {
    const RDGraphPoint* path = nullptr;
    usize c = rdgraph_getarrow(m_graph, &e, &path);
    QPolygon arrowhead;

    for(usize i = 0; i < c; i++) {
        const RDGraphPoint& p = path[i];
        arrowhead << QPoint{p.x, p.y};
    }

    m_arrows[e] = arrowhead;
}

void GraphView::precompute_line(const RDGraphEdge& e) {
    const RDGraphPoint* path = nullptr;
    usize c = rdgraph_getroutes(m_graph, &e, &path);
    QVector<QLine> lines;

    for(usize i = 0; c && (i < c - 1); i++) {
        const RDGraphPoint& p1 = path[i];
        const RDGraphPoint& p2 = path[i + 1];
        lines.push_back(QLine{p1.x, p1.y, p2.x, p2.y});
    }

    m_lines[e] = lines;
}

bool GraphView::update_selected_item(QMouseEvent* e, QPoint* itempos) {
    GraphViewItem* olditem = m_selecteditem;
    m_selecteditem = this->item_from_mouse_event(e, itempos);

    if(olditem) {
        olditem->itemselection_changed(false);
        olditem->invalidate();
    }

    if(m_selecteditem) {
        m_selecteditem->itemselection_changed(true);
        m_selecteditem->invalidate();
    }

    return olditem != m_selecteditem;
}
