#include "surfacesplitview.h"
#include "../../fontawesome.h"
#include "../../themeprovider.h"

SurfaceSplitView::SurfaceSplitView(QWidget* parent)
    : SplitView{new QStackedWidget(), parent} {

    QAction* actback = this->add_button(
        FA_ICON_COLOR(0xf053, themeprovider::color(THEME_GRAPHEDGELOOPCOND)));
    QAction* actforward = this->add_button(
        FA_ICON_COLOR(0xf054, themeprovider::color(THEME_GRAPHEDGELOOPCOND)));
    QAction* actgoto = this->add_button(FA_ICON(0xf1e5));

    m_surfaceview = new SurfaceView();
    m_surfacegraph = new SurfaceGraph();

    this->stacked_widget()->addWidget(m_surfaceview);
    this->stacked_widget()->addWidget(m_surfacegraph);

    connect(m_surfaceview->viewport(), &SurfaceWidget::switch_view, this,
            [&]() {
                this->stacked_widget()->setCurrentWidget(m_surfacegraph);
                m_surfacegraph->set_location(m_surfaceview->location());
            });

    connect(m_surfacegraph, &SurfaceGraph::switch_view, this, [&]() {
        this->stacked_widget()->setCurrentWidget(m_surfaceview);
        m_surfaceview->set_location(m_surfacegraph->location());
    });

    connect(actback, &QAction::triggered, this, [=]() {});
    connect(actforward, &QAction::triggered, this, [=]() {});
    connect(actgoto, &QAction::triggered, this, [=]() {});
}

RDSurface* SurfaceSplitView::handle() const {
    if(this->stacked_widget()->currentWidget() == m_surfacegraph)
        return m_surfacegraph->handle();

    return m_surfaceview->handle();
}

void SurfaceSplitView::jump_to(RDAddress address) { // NO LINT
    if(this->stacked_widget()->currentWidget() == m_surfacegraph) {
        m_surfacegraph->jump_to(address);
        m_surfacegraph->setFocus();
    }
    else {
        m_surfaceview->jump_to(address);
        m_surfaceview->setFocus();
    }
}

void SurfaceSplitView::invalidate() {

    if(this->stacked_widget()->currentWidget() == m_surfacegraph)
        m_surfacegraph->invalidate();
    else
        m_surfaceview->invalidate();
}

SplitView* SurfaceSplitView::create_split() const { return nullptr; }
