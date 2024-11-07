#include "surfacesplitview.h"
#include "../../actions.h"
#include "../../fontawesome.h"
#include "../../themeprovider.h"
#include ".././splitview/splitwidget.h"

namespace {

SurfaceView* splitwidget_getcurrentview(SplitWidget* split) {
    if(!split)
        return nullptr;

    auto* stackw = qobject_cast<QStackedWidget*>(split->widget());

    if(stackw)
        return qobject_cast<SurfaceView*>(stackw->currentWidget());

    return nullptr;
}

SurfaceGraph* splitwidget_getcurrentgraph(SplitWidget* split) {
    if(!split)
        return nullptr;

    auto* stackw = qobject_cast<QStackedWidget*>(split->widget());

    if(stackw)
        return qobject_cast<SurfaceGraph*>(stackw->currentWidget());

    return nullptr;
}

SurfaceView* splitwidget_findsurfaceview(SplitWidget* split) {
    if(!split)
        return nullptr;

    auto* stackw = qobject_cast<QStackedWidget*>(split->widget());

    if(stackw) {
        for(int i = 0; i < stackw->count(); i++) {
            if(auto* w = qobject_cast<SurfaceView*>(stackw->widget(i)); w)
                return w;
        }
    }

    return nullptr;
}

SurfaceGraph* splitwidget_findsurfacegraph(SplitWidget* split) {
    if(!split)
        return nullptr;

    auto* stackw = qobject_cast<QStackedWidget*>(split->widget());

    if(stackw) {
        for(int i = 0; i < stackw->count(); i++) {
            if(auto* w = qobject_cast<SurfaceGraph*>(stackw->widget(i)); w)
                return w;
        }
    }

    return nullptr;
}

} // namespace

SurfaceSplitDelegate::SurfaceSplitDelegate(QObject* parent)
    : SplitDelegate{parent} {}

QWidget* SurfaceSplitDelegate::create_widget(SplitWidget* split,
                                             SplitWidget* current) {
    QAction* actback = split->add_button(
        FA_ICON_COLOR(0xf053, themeprovider::color(THEME_GRAPHEDGELOOPCOND)));
    QAction* actforward = split->add_button(
        FA_ICON_COLOR(0xf054, themeprovider::color(THEME_GRAPHEDGELOOPCOND)));
    split->add_button(actions::get(actions::GOTO));

    auto* stack = new QStackedWidget();
    auto* surfaceview = new SurfaceView();
    auto* surfacegraph = new SurfaceGraph();
    stack->addWidget(surfaceview);
    stack->addWidget(surfacegraph);

    // Initialize Actions
    actback->setEnabled(surfaceview->viewport()->can_goback());
    actforward->setEnabled(surfaceview->viewport()->can_goforward());

    // Sync view location
    if(SurfaceView* cview = splitwidget_findsurfaceview(current); cview)
        surfaceview->set_location(cview->location());

    // Sync graph location
    if(SurfaceGraph* cgraph = splitwidget_findsurfacegraph(current); cgraph)
        surfacegraph->set_location(cgraph->location());

    connect(surfaceview->viewport(), &SurfaceWidget::history_updated,
            [surfaceview, actback, actforward]() {
                if(surfaceview->isVisible()) { // Ignore spurious signals
                    actback->setEnabled(surfaceview->viewport()->can_goback());
                    actforward->setEnabled(
                        surfaceview->viewport()->can_goforward());
                }
            });

    connect(surfacegraph, &SurfaceGraph::history_updated,
            [surfacegraph, actback, actforward]() {
                if(surfacegraph->isVisible()) { // Ignore spurious signals
                    actback->setEnabled(surfacegraph->can_goback());
                    actforward->setEnabled(surfacegraph->can_goforward());
                }
            });

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

    connect(actback, &QAction::triggered, this, [split]() {
        if(SurfaceView* v = splitwidget_getcurrentview(split); v)
            v->viewport()->go_back();
        else if(SurfaceGraph* g = splitwidget_getcurrentgraph(split); g)
            g->go_back();
    });

    connect(actforward, &QAction::triggered, this, [split]() {
        if(SurfaceView* v = splitwidget_getcurrentview(split); v)
            v->viewport()->go_forward();
        else if(SurfaceGraph* g = splitwidget_getcurrentgraph(split); g)
            g->go_forward();
    });

    return stack;
}

SurfaceSplitView::SurfaceSplitView(QWidget* parent)
    : SplitView{new SurfaceSplitDelegate(), parent} {}

RDSurface* SurfaceSplitView::handle() const {
    auto* stackw =
        qobject_cast<QStackedWidget*>(this->current_split()->widget());

    if(stackw) {
        QWidget* w = stackw->currentWidget();

        if(auto* sv = qobject_cast<SurfaceView*>(w); sv)
            return sv->handle();

        if(auto* sg = qobject_cast<SurfaceGraph*>(w); sg)
            return sg->handle();
    }

    return nullptr;
}

bool SurfaceSplitView::has_rdil() const {
    auto* stackw =
        qobject_cast<QStackedWidget*>(this->current_split()->widget());

    if(stackw) {
        QWidget* w = stackw->currentWidget();

        if(auto* sv = qobject_cast<SurfaceView*>(w); sv)
            return sv->has_rdil();
        if(auto* sg = qobject_cast<SurfaceGraph*>(w); sg)
            return sg->has_rdil();
    }

    return false;
}

void SurfaceSplitView::set_rdil(bool b) {
    auto* stackw =
        qobject_cast<QStackedWidget*>(this->current_split()->widget());

    if(stackw) {
        QWidget* w = stackw->currentWidget();

        if(auto* sv = qobject_cast<SurfaceView*>(w); sv)
            sv->set_rdil(b);
        else if(auto* sg = qobject_cast<SurfaceGraph*>(w); sg)
            sg->set_rdil(b);
    }
}

void SurfaceSplitView::jump_to(RDAddress address) { // NO LINT
    auto* stackw =
        qobject_cast<QStackedWidget*>(this->current_split()->widget());

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
        qobject_cast<QStackedWidget*>(this->current_split()->widget());

    if(stackw) {
        QWidget* w = stackw->currentWidget();

        if(auto* sv = qobject_cast<SurfaceView*>(w); sv)
            sv->invalidate();
        else if(auto* sg = qobject_cast<SurfaceGraph*>(w); sg)
            sg->invalidate();
    }
}

SurfaceView* SurfaceSplitView::surface_view() const {
    return splitwidget_findsurfaceview(this->current_split());
}

SurfaceGraph* SurfaceSplitView::surface_graph() const {
    return splitwidget_findsurfacegraph(this->current_split());
}
