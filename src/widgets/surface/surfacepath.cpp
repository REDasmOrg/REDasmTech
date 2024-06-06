#include "surfacepath.h"
#include "../../themeprovider.h"
#include "surfacewidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QRandomGenerator>

SurfacePath::SurfacePath(SurfaceWidget* surface, QWidget* parent)
    : QWidget{parent}, m_surface{surface} {
    this->setBackgroundRole(QPalette::Base);
    this->setAutoFillBackground(true);
    this->setFont(surface->font());

    connect(surface, &SurfaceWidget::render_completed, this,
            qOverload<>(&SurfacePath::update));
}

void SurfacePath::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);

    const RDSurfacePath* path = nullptr;
    usize c = rdsurface_getpath(m_surface->handle(), &path);
    if(!c)
        return;

    QPainter painter{this};
    QFontMetrics fm = this->fontMetrics();

    int rows = m_surface->visible_rows();
    int w = fm.horizontalAdvance(" ");
    qreal h = m_surface->row_height(), x = this->width() - (w * 2);

    for(usize i = 0; i < c; i++, x -= w, path++) {
        int y1 = (path->fromrow * h) + (h / 2);
        int y2 = (std::min(path->torow, rows + 1) * h) + (h / 2);
        int y = (std::min(path->torow, rows + 1) * h);
        qreal penwidth = this->is_path_selected(path) ? 3 : 2;

        if(y2 > (y + (h / 2)))
            y2 -= penwidth;
        else if(y2 < (y + (h / 2)))
            y2 += penwidth;

        QVector<QLine> points;
        points.push_back(QLine(this->width(), y1, x, y1));
        points.push_back(QLine(x, y1, x, y2));
        points.push_back(QLine(x, y2, this->width(), y2));

        Qt::PenStyle penstyle = ((path->fromrow == -1) || (path->torow > rows))
                                    ? Qt::DotLine
                                    : Qt::SolidLine;
        painter.setPen(
            QPen{themeprovider::color(path->style), penwidth, penstyle});
        painter.drawLines(points);

        painter.setPen(
            QPen{themeprovider::color(path->style), penwidth, Qt::SolidLine});
        this->fill_arrow(&painter, y2, fm);
    }
}

bool SurfacePath::is_path_selected(const RDSurfacePath* p) const {
    int row = static_cast<int>(m_surface->position().row);
    return (row == p->fromrow) || (row == p->torow);
}

void SurfacePath::fill_arrow(QPainter* painter, int y, const QFontMetrics& fm) {
    const int W = fm.horizontalAdvance(" ") / 2;
    const int HL = fm.height() / 3;

    QPainterPath path;
    path.moveTo(QPoint{this->width() - W, y});
    path.lineTo(QPoint{this->width() - W, y - HL});
    path.lineTo(QPoint{this->width(), y});
    path.lineTo(QPoint{this->width() - W, y + HL});
    path.lineTo(QPoint{this->width() - W, y});

    painter->fillPath(path, painter->pen().brush());
}
