#include "surfacepath.h"
#include "surfacewidget.h"
#include <QPainter>

SurfacePath::SurfacePath(SurfaceWidget* surface, QWidget* parent)
    : QWidget{parent}, m_surface{surface} {
    this->setBackgroundRole(QPalette::Base);
    this->setAutoFillBackground(true);
}

void SurfacePath::paintEvent(QPaintEvent* event) { QWidget::paintEvent(event); }
