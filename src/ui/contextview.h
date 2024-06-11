#pragma once

#include "../views/surface/surfacegraph.h"
#include "../views/surface/surfaceview.h"
#include <QHeaderView>
#include <QSplitter>
#include <QStackedWidget>
#include <QTreeView>
#include <QVBoxLayout>

namespace ui {

struct ContextView {
    QSplitter* splitter;
    SurfaceView* surfaceview;
    SurfaceGraph* surfacegraph;
    QStackedWidget* stackedview;
    QTreeView* tvfunctions;

    explicit ContextView(QWidget* self) {
        this->surfaceview = new SurfaceView();
        this->surfacegraph = new SurfaceGraph();

        this->stackedview = new QStackedWidget();
        this->stackedview->addWidget(this->surfaceview);
        this->stackedview->addWidget(this->surfacegraph);

        this->tvfunctions = new QTreeView();
        this->tvfunctions->header()->setStretchLastSection(true);
        this->tvfunctions->setUniformRowHeights(true);
        this->tvfunctions->setRootIsDecorated(false);
        this->tvfunctions->setFrameShape(QFrame::NoFrame);

        this->splitter = new QSplitter();
        this->splitter->addWidget(this->tvfunctions);
        this->splitter->addWidget(this->stackedview);
        this->splitter->setStretchFactor(0, 20);
        this->splitter->setStretchFactor(1, 80);

        auto* vbox = new QVBoxLayout(self);
        vbox->setContentsMargins(0, 0, 0, 0);
        vbox->addWidget(this->splitter);
    }
};

} // namespace ui
