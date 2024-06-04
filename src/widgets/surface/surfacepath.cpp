#include "surfacepath.h"
#include "surfacewidget.h"
#include <QPainter>

SurfacePath::SurfacePath(SurfaceWidget* surface, QWidget* parent)
    : QWidget{parent}, m_surface{surface} {
    this->setBackgroundRole(QPalette::Base);
    this->setAutoFillBackground(true);

    connect(surface, &SurfaceWidget::render_completed, this,
            qOverload<>(&SurfacePath::update));
}

void SurfacePath::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);

    const RDSurfacePath* path = nullptr;
    usize c = rdsurface_getpath(m_surface->handle(), &path);
}
