#include "surfacepopup.h"
#include "../../settings.h"
#include "../../utils.h"
#include <QPainter>
#include <QWheelEvent>

namespace {

constexpr int POPUP_MARGIN = 16;

}

SurfacePopup::SurfacePopup(QWidget* parent): QWidget{parent} {
    this->setFont(REDasmSettings::font());
    this->setCursor(Qt::ArrowCursor);
    this->setBackgroundRole(QPalette::ToolTipBase);
    this->setAutoFillBackground(true);

    this->setWindowFlags(Qt::Popup);
    this->setMouseTracking(true);
    this->setMinimumHeight(0);
    this->setMinimumWidth(0);

    m_document.setDefaultFont(this->font());
    m_document.setUndoRedoEnabled(false);

    m_surface = rdsurface_new(SURFACE_POPUP);
}

SurfacePopup::~SurfacePopup() { rd_destroy(m_surface); }

bool SurfacePopup::popup(RDAddress address) {
    LIndex index;

    if(!rdlisting_getindex(address, &index)) {
        this->hide();
        return false;
    }

    rdsurface_seek(m_surface, index);

    QPoint pt = QCursor::pos();
    pt.rx() += POPUP_MARGIN;
    pt.ry() += POPUP_MARGIN;
    this->move(pt);
    this->render();
    this->show();
    return true;
}

void SurfacePopup::more_rows() {
    m_nrows++;
    this->render();
}

void SurfacePopup::less_rows() {
    if(m_nrows == 1) return;

    m_nrows--;
    this->render();
}

void SurfacePopup::render() {
    if(!m_surface) return;

    rdsurface_render(m_surface, m_nrows);
    utils::draw_surface(m_surface, &m_document, 0, m_nrows);

    this->resize(m_document.size().toSize());
    this->update();
}

void SurfacePopup::mouseMoveEvent(QMouseEvent* event) {
    if(m_lastpos != event->globalPosition()) {
        this->hide();
        event->accept();
    }
    else
        QWidget::mouseMoveEvent(event);
}

void SurfacePopup::wheelEvent(QWheelEvent* event) {
    m_lastpos = event->globalPosition();
    QPoint delta = event->angleDelta();

    if(delta.y() > 0)
        this->less_rows();
    else
        this->more_rows();

    event->accept();
}

void SurfacePopup::paintEvent(QPaintEvent* e) {
    if(!m_surface) {
        QWidget::paintEvent(e);
        return;
    }

    QPainter painter{this};
    m_document.drawContents(&painter);
}
