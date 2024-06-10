#include "graphviewitem.h"

GraphViewItem::GraphViewItem(RDGraphNode node, const RDGraph* g,
                             QObject* parent)
    : QObject{parent}, m_node{node}, m_graph{g} {}

const RDGraph* GraphViewItem::graph() const { return m_graph; }
RDGraphNode GraphViewItem::node() const { return m_node; }
int GraphViewItem::x() const { return this->position().x(); }
int GraphViewItem::y() const { return this->position().y(); }
int GraphViewItem::width() const { return this->size().width(); }
int GraphViewItem::height() const { return this->size().height(); }
QRect GraphViewItem::rect() const { return {m_pos, this->size()}; }

bool GraphViewItem::contains(const QPoint& p) const {
    return this->rect().contains(p);
}

const QPoint& GraphViewItem::position() const { return m_pos; }
void GraphViewItem::move(const QPoint& pos) { m_pos = pos; }

void GraphViewItem::itemselection_changed(bool selected) { Q_UNUSED(selected); }

QPoint GraphViewItem::map_to_item(const QPoint& p) const {
    return {p.x() - m_pos.x(), p.y() - m_pos.y()};
}

int GraphViewItem::current_row() const { return 0; }
void GraphViewItem::mousedoubleclick_event(QMouseEvent* e) { Q_UNUSED(e); }
void GraphViewItem::mousepress_event(QMouseEvent* e) { Q_UNUSED(e); }
void GraphViewItem::mousemove_event(QMouseEvent* e) { Q_UNUSED(e); }

void GraphViewItem::invalidate(bool notify) {
    if(notify)
        Q_EMIT invalidated();
}
