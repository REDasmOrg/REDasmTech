#include "surfacegraphitem.h"
#include <QApplication>
#include <QPainter>
#include <QWidget>

namespace {

constexpr int DROP_SHADOW_SIZE = 10;

}

SurfaceGraphItem::SurfaceGraphItem(RDSurface* surface,
                                   const RDFunctionBasicBlock& fbb,
                                   RDGraphNode n, RDFunction* f,
                                   QWidget* parent)
    : GraphViewItem{n, rdfunction_getgraph(f), parent}, m_basicblock{fbb},
      m_surface{surface} {}

bool SurfaceGraphItem::contains(RDAddress address) const {
    return rdfunction_contains(m_function, address);
}

int SurfaceGraphItem::current_row() const {
    // RDAddress address = m_surface->currentAddress();

    // if((address != RD_NVAL) && this->contains(address))
    // return m_surface->position().row - this->start_row();

    return GraphViewItem::current_row();
}

QSize SurfaceGraphItem::size() const {
    return {}; // m_surface->rangeSize(
               //  RDFunctionBasicBlock_GetStartAddress(m_basicblock),
               //  RDFunctionBasicBlock_GetEndAddress(m_basicblock));
}

void SurfaceGraphItem::itemselection_changed(bool selected) {
    // m_surface->activateCursor(selected);
}

void SurfaceGraphItem::mousedoubleclick_event(QMouseEvent*) {
    Q_EMIT follow_requested();
}

void SurfaceGraphItem::mousepress_event(QMouseEvent* e) {
    if(e->buttons() == Qt::LeftButton) {
        usize row, col;
        this->localpos_to_surface(e->position(), &row, &col);
        rdsurface_setposition(m_surface, row, col);
    }
    else
        GraphViewItem::mousepress_event(e);

    e->accept();
}

void SurfaceGraphItem::mousemove_event(QMouseEvent* e) {
    if(e->buttons() != Qt::LeftButton)
        return;

    usize row, col;
    this->localpos_to_surface(e->position(), &row, &col);
    rdsurface_select(m_surface, row, col);
    e->accept();
}

void SurfaceGraphItem::localpos_to_surface(const QPointF& pt, usize* row,
                                           usize* col) const {
    // *row = this->start_row() + (pt.y() / m_surface->cellHeight());
    // *col = (pt.x() / m_surface->cellWidth());
}

int SurfaceGraphItem::start_row() const {
    // return m_surface->indexOf(
    //     RDFunctionBasicBlock_GetStartAddress(m_basicblock));

    return -1;
}

void SurfaceGraphItem::render(QPainter* painter, usize state) {
    QRect r(QPoint(0, 0), this->size());
    r.adjust(BLOCK_MARGINS);

    QColor shadow = painter->pen().color();
    shadow.setAlpha(127);

    painter->save();
    painter->translate(this->position());

    if(state & SurfaceGraphItem::SELECTED) // Thicker shadow
        painter->fillRect(r.adjusted(DROP_SHADOW_SIZE, DROP_SHADOW_SIZE,
                                     DROP_SHADOW_SIZE + 2,
                                     DROP_SHADOW_SIZE + 2),
                          shadow);
    else
        painter->fillRect(r.adjusted(DROP_SHADOW_SIZE, DROP_SHADOW_SIZE,
                                     DROP_SHADOW_SIZE, DROP_SHADOW_SIZE),
                          shadow);

    painter->fillRect(r, qApp->palette().color(QPalette::Base));

    if(m_surface) {
        painter->setClipRect(r);
        // m_surface->renderRange(
        // painter, RDFunctionBasicBlock_GetStartAddress(m_basicblock),
        // RDFunctionBasicBlock_GetEndAddress(m_basicblock));
    }

    painter->setClipping(false);

    if(state & SurfaceGraphItem::SELECTED)
        painter->setPen(QPen(qApp->palette().color(QPalette::Highlight), 2.0));
    else
        painter->setPen(QPen(qApp->palette().color(QPalette::WindowText), 1.5));

    painter->drawRect(r);
    painter->restore();
}
