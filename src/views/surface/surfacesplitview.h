#pragma once

#include "../splitview.h"
#include "surfacegraph.h"
#include "surfaceview.h"
#include <QStackedWidget>

class SurfaceSplitView: public SplitView {
    Q_OBJECT

public:
    explicit SurfaceSplitView(QWidget* parent = nullptr);
    [[nodiscard]] RDSurface* handle() const;
    void jump_to(RDAddress address);
    void invalidate();

    [[nodiscard]] inline SurfaceView* surface_view() const {
        return m_surfaceview;
    }

    [[nodiscard]] inline SurfaceGraph* surface_graph() const {
        return m_surfacegraph;
    }

private:
    [[nodiscard]] SplitView* create_split() const override;

    [[nodiscard]] inline QStackedWidget* stacked_widget() const {
        return static_cast<QStackedWidget*>(this->split_view());
    }

private:
    SurfaceView* m_surfaceview;
    SurfaceGraph* m_surfacegraph;
};
