#pragma once

#include <QWidget>
#include <redasm/redasm.h>

class SurfaceWidget;

class SurfacePath: public QWidget {
    Q_OBJECT

public:
    explicit SurfacePath(SurfaceWidget* surface, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    [[nodiscard]] bool is_path_selected(const RDSurfacePath* p) const;
    void fill_arrow(QPainter* painter, int y, const QFontMetrics& fm);

private:
    SurfaceWidget* m_surface;
};
