#include "surfacegraphnode.h"
#include "../../settings.h"
#include "../../utils.h"
#include <QApplication>
#include <QPainter>
#include <QWidget>

namespace {

constexpr int DROP_SHADOW_SIZE = 10;

}

SurfaceGraphNode::SurfaceGraphNode(RDSurface* surface, const RDBasicBlock* bb,
                                   RDGraphNode n, RDFunction* f,
                                   QWidget* parent)
    : GraphViewNode{n, rdfunction_getgraph(f), parent}, m_basicblock{bb},
      m_surface{surface} {
    m_document.setDefaultFont(REDasmSettings::font());
    m_document.setUndoRedoEnabled(false);
    m_document.setDocumentMargin(0);
    this->update_document();
}

bool SurfaceGraphNode::contains_address(RDAddress address) const {
    return address >= m_basicblock->start && address <= m_basicblock->end;
}

int SurfaceGraphNode::current_row() const {
    RDSurfaceLocation loc;
    rdsurface_getlocation(m_surface, &loc);

    if(loc.address.valid && this->contains_address(loc.address.value)) {
        RDSurfacePosition pos;
        rdsurface_getposition(m_surface, &pos);
        return pos.row - this->start_row();
    }

    return GraphViewNode::current_row();
}

QSize SurfaceGraphNode::size() const {
    QSize sz = m_document.size().toSize();
    sz.setWidth(m_maxwidth); // Scale to longest row
    return sz;
}

void SurfaceGraphNode::mousedoubleclick_event(QMouseEvent*) {
    Q_EMIT follow_requested();
}

void SurfaceGraphNode::mousepress_event(QMouseEvent* e) {
    if(e->buttons() == Qt::LeftButton) {
        RDSurfacePosition pos;
        this->get_surface_pos(e->position(), &pos);
        rdsurface_setposition(m_surface, pos.row, pos.col);
        this->invalidate();
    }
    else
        GraphViewNode::mousepress_event(e);

    e->accept();
}

void SurfaceGraphNode::mousemove_event(QMouseEvent* e) {
    if(e->buttons() != Qt::LeftButton) return;

    RDSurfacePosition pos;
    this->get_surface_pos(e->position(), &pos);
    rdsurface_select(m_surface, pos.row, pos.col);
    this->invalidate();
    e->accept();
}

void SurfaceGraphNode::update_document() {
    if(m_surface) {
        int startidx = rdsurface_indexof(m_surface, m_basicblock->start);
        int endidx = rdsurface_lastindexof(m_surface, m_basicblock->end);
        if(startidx == -1 || endidx == -1) return;

        usize nmaxcol = utils::draw_surface(m_surface, &m_document, startidx,
                                            endidx - startidx + 1);
        m_maxwidth = nmaxcol * utils::cell_width();
    }
    else
        m_document.clear();
}

void SurfaceGraphNode::get_surface_pos(const QPointF& pt,
                                       RDSurfacePosition* pos) const {
    pos->row = this->start_row() + (pt.y() / utils::cell_height());
    pos->col = (pt.x() / utils::cell_width());
}

int SurfaceGraphNode::start_row() const {
    return rdsurface_indexof(m_surface, m_basicblock->start);
}

void SurfaceGraphNode::render(QPainter* painter, usize state) {
    QRect r{QPoint{}, this->size()};
    r.adjust(BLOCK_MARGINS);

    QColor shadow = painter->pen().color();
    shadow.setAlpha(127);

    painter->save();
    painter->translate(this->position());

    if(state & SurfaceGraphNode::SELECTED) // Thicker shadow
        painter->fillRect(r.adjusted(DROP_SHADOW_SIZE, DROP_SHADOW_SIZE,
                                     DROP_SHADOW_SIZE + 2,
                                     DROP_SHADOW_SIZE + 2),
                          shadow);
    else
        painter->fillRect(r.adjusted(DROP_SHADOW_SIZE, DROP_SHADOW_SIZE,
                                     DROP_SHADOW_SIZE, DROP_SHADOW_SIZE),
                          shadow);

    painter->fillRect(r, qApp->palette().color(QPalette::Base));
    painter->setClipRect(r);
    m_document.drawContents(painter);
    painter->setClipping(false);

    if(state & SurfaceGraphNode::SELECTED)
        painter->setPen(QPen{qApp->palette().color(QPalette::Highlight), 2.0});
    else
        painter->setPen(QPen{qApp->palette().color(QPalette::WindowText), 1.5});

    painter->drawRect(r);
    painter->restore();
}
