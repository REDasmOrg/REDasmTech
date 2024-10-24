#include "surfacesplitview.h"
#include "../../fontawesome.h"
#include "../../themeprovider.h"
#include ".././splitview/splitwidget.h"

SurfaceSplitDelegate::SurfaceSplitDelegate(QObject* parent)
    : SplitDelegate{parent} {}

QWidget* SurfaceSplitDelegate::create_widget(SplitWidget* split) {
    QAction* actback = split->add_button(
        FA_ICON_COLOR(0xf053, themeprovider::color(THEME_GRAPHEDGELOOPCOND)));
    QAction* actforward = split->add_button(
        FA_ICON_COLOR(0xf054, themeprovider::color(THEME_GRAPHEDGELOOPCOND)));
    QAction* actgoto = split->add_button(FA_ICON(0xf1e5));

    auto* stack = new QStackedWidget();
    auto* surfaceview = new SurfaceView();
    auto* surfacegraph = new SurfaceGraph();
    stack->addWidget(surfaceview);
    stack->addWidget(surfacegraph);

    connect(surfaceview->viewport(), &SurfaceWidget::switch_view, this,
            [stack, surfaceview, surfacegraph]() {
                stack->setCurrentWidget(surfacegraph);
                surfacegraph->set_location(surfaceview->location());
            });

    connect(surfacegraph, &SurfaceGraph::switch_view, this,
            [stack, surfaceview, surfacegraph]() {
                stack->setCurrentWidget(surfaceview);
                surfaceview->set_location(surfacegraph->location());
            });

    connect(actback, &QAction::triggered, this, [=]() {});
    connect(actforward, &QAction::triggered, this, [=]() {});
    connect(actgoto, &QAction::triggered, this, [=]() {});

    return stack;
}

SurfaceSplitView::SurfaceSplitView(QWidget* parent)
    : SplitView{new SurfaceSplitDelegate(), parent} {}

RDSurface* SurfaceSplitView::handle() const {
    auto* stackw =
        qobject_cast<QStackedWidget*>(this->selected_split()->widget());

    if(stackw) {
        QWidget* w = stackw->currentWidget();

        if(auto* sv = qobject_cast<SurfaceView*>(w); sv)
            return sv->handle();

        if(auto* sg = qobject_cast<SurfaceGraph*>(w); sg)
            return sg->handle();
    }

    return nullptr;
}

void SurfaceSplitView::jump_to(RDAddress address) { // NO LINT
    auto* stackw =
        qobject_cast<QStackedWidget*>(this->selected_split()->widget());

    if(stackw) {
        QWidget* w = stackw->currentWidget();

        if(auto* sv = qobject_cast<SurfaceView*>(w); sv) {
            sv->jump_to(address);
            sv->setFocus();
        }
        else if(auto* sg = qobject_cast<SurfaceGraph*>(w); sg) {
            sg->jump_to(address);
            sg->setFocus();
        }
    }
}

void SurfaceSplitView::invalidate() {
    auto* stackw =
        qobject_cast<QStackedWidget*>(this->selected_split()->widget());

    if(stackw) {
        QWidget* w = stackw->currentWidget();

        if(auto* sv = qobject_cast<SurfaceView*>(w); sv)
            sv->invalidate();
        else if(auto* sg = qobject_cast<SurfaceGraph*>(w); sg)
            sg->invalidate();
    }
}

SurfaceView* SurfaceSplitView::surface_view() const {
    auto* stackw =
        qobject_cast<QStackedWidget*>(this->selected_split()->widget());

    qDebug() << this->selected_split()->widget();

    if(stackw) {
        for(int i = 0; i < stackw->count(); i++) {
            if(auto* w = qobject_cast<SurfaceView*>(stackw->widget(i)); w)
                return w;
        }
    }

    return nullptr;
}

SurfaceGraph* SurfaceSplitView::surface_graph() const {
    auto* stackw =
        qobject_cast<QStackedWidget*>(this->selected_split()->widget());

    if(stackw) {
        for(int i = 0; i < stackw->count(); i++) {
            if(auto* w = qobject_cast<SurfaceGraph*>(stackw->widget(i)); w)
                return w;
        }
    }

    return nullptr;
}
