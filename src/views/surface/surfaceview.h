#pragma once

#include "surfacepath.h"
#include "surfacewidget.h"
#include <QSplitter>

class SurfaceView: public QSplitter {
    Q_OBJECT

public:
    explicit SurfaceView(QWidget* parent = nullptr);
    void clear_history() { m_surface->clear_history(); }
    void jump_to_ep() { m_surface->jump_to_ep(); }
    void jump_to(RDAddress address) { m_surface->jump_to(address); }
    void invalidate() { m_surface->invalidate(); }
    auto location() { return m_surface->location(); }
    [[nodiscard]] auto viewport() const { return m_surface; }
    [[nodiscard]] auto handle() const { return m_surface->handle(); }
    [[nodiscard]] bool has_rdil() const { return m_surface->has_rdil(); }
    void set_rdil(bool v) const { m_surface->set_rdil(v); }

    void set_location(const RDSurfaceLocation& loc) {
        m_surface->set_location(loc);
    }

private:
    SurfacePath* m_surfacepath;
    SurfaceWidget* m_surface;
};
