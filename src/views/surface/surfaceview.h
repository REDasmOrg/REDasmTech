#pragma once

#include "surfacepath.h"
#include "surfacewidget.h"
#include <QSplitter>

class SurfaceView: public QSplitter {
    Q_OBJECT

public:
    explicit SurfaceView(QWidget* parent = nullptr);
    inline void jump_to_ep() { m_surface->jump_to_ep(); }
    inline void jump_to(RDAddress address) { m_surface->jump_to(address); }
    inline void invalidate() { m_surface->invalidate(); }
    inline auto location() { return m_surface->location(); }
    [[nodiscard]] inline auto viewport() const { return m_surface; }
    [[nodiscard]] inline auto handle() const { return m_surface->handle(); }

    inline void set_location(const RDSurfaceLocation& loc) {
        m_surface->set_location(loc);
    }

private:
    SurfacePath* m_surfacepath;
    SurfaceWidget* m_surface;
};
