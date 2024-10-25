#pragma once

#include "../splitview/splitview.h"
#include "surfacegraph.h"
#include "surfaceview.h"
#include <QStackedWidget>

class SurfaceSplitDelegate: public SplitDelegate {
    Q_OBJECT

public:
    explicit SurfaceSplitDelegate(QObject* parent = nullptr);
    QWidget* create_widget(SplitWidget* split, SplitWidget* current) override;
};

class SurfaceSplitView: public SplitView {
    Q_OBJECT

public:
    explicit SurfaceSplitView(QWidget* parent = nullptr);
    [[nodiscard]] RDSurface* handle() const;
    void jump_to(RDAddress address);
    void invalidate();
    [[nodiscard]] SurfaceView* surface_view() const;
    [[nodiscard]] SurfaceGraph* surface_graph() const;
};
