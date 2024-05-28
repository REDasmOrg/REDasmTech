#pragma once

#include "surfacepath.h"
#include "surfacewidget.h"
#include <QSplitter>

class SurfaceView: public QSplitter {
    Q_OBJECT

public:
    explicit SurfaceView(QWidget* parent = nullptr);
    inline void jump_to(size_t address) { m_surface->jump_to(address); }

    [[nodiscard]] inline RDSurface* handle() const {
        return m_surface->handle();
    }

private:
    SurfacePath* m_surfacepath;
    SurfaceWidget* m_surface;
};
