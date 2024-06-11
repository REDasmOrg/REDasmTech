#include "surfaceview.h"
#include "../../themeprovider.h"
#include <QKeyEvent>
#include <redasm/redasm.h>

SurfaceView::SurfaceView(QWidget* parent): QSplitter{parent} {
    this->setStyleSheet("QSplitter::handle { background-color: " +
                        themeprovider::color(THEME_SEEK).name() + "; }");

    this->setFocusPolicy(Qt::NoFocus);

    m_surface = new SurfaceWidget();
    m_surfacepath = new SurfacePath(m_surface);

    this->addWidget(m_surfacepath);
    this->addWidget(m_surface);

    // Shrink the left side
    auto w = qCeil(m_surface->cell_width()) * 8;
    this->setSizes({w, this->width() - w});
    this->setHandleWidth(4);
}
