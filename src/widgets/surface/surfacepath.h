#pragma once

#include <QWidget>

class SurfaceWidget;

class SurfacePath: public QWidget {
    Q_OBJECT

public:
    explicit SurfacePath(SurfaceWidget* surface, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    SurfaceWidget* m_surface;
};
