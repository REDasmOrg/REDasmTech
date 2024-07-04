#include "contextview.h"
#include "../statusbar.h"

ContextView::ContextView(QWidget* parent): QWidget{parent}, m_ui{this} {
    m_functionsmodel = new SymbolsFilterModel(SYMBOL_FUNCTION, this);
    m_functionsmodel->symbols_model()->set_highlight_address(true);
    m_ui.tvfunctions->setModel(m_functionsmodel);
    m_ui.tvfunctions->header()->hideSection(1);
    m_ui.tvfunctions->header()->setSectionResizeMode(
        0, QHeaderView::ResizeToContents);

    connect(m_ui.tvfunctions, &QTreeView::doubleClicked, this,
            [&](const QModelIndex& index) {
                RDAddress address = m_functionsmodel->address(index);
                this->jump_to(address);
            });

    connect(m_ui.surfaceview->viewport(), &SurfaceWidget::switch_view, this,
            [&]() {
                m_ui.stackedview->setCurrentWidget(m_ui.surfacegraph);
                m_ui.surfacegraph->set_location(m_ui.surfaceview->location());
            });

    connect(m_ui.surfacegraph, &SurfaceGraph::switch_view, this, [&]() {
        m_ui.stackedview->setCurrentWidget(m_ui.surfaceview);
        m_ui.surfaceview->set_location(m_ui.surfacegraph->location());
    });

    connect(statusbar::status_icon(), &QPushButton::clicked, this,
            [&]() { m_active = !m_active; });
}

ContextView::~ContextView() { rd_destroy(); }

RDSurface* ContextView::handle() const {
    if(m_ui.stackedview->currentWidget() == m_ui.surfacegraph)
        return m_ui.surfacegraph->handle();

    return m_ui.surfaceview->handle();
}

void ContextView::tick(const RDEngineStatus* s) {
    if(s->step == STEP_EMULATE)
        return;

    m_functionsmodel->resync();
    this->invalidate();

    if(s->step == STEP_DONE)
        m_ui.surfaceview->jump_to_ep();
}

void ContextView::jump_to(RDAddress address) { // NOLINT
    if(m_ui.stackedview->currentWidget() == m_ui.surfacegraph) {
        m_ui.surfacegraph->jump_to(address);
        m_ui.surfacegraph->setFocus();
    }
    else {
        m_ui.surfaceview->jump_to(address);
        m_ui.surfaceview->setFocus();
    }
}

void ContextView::invalidate() { // NOLINT
    if(m_ui.stackedview->currentWidget() == m_ui.surfacegraph)
        m_ui.surfacegraph->invalidate();
    else
        m_ui.surfaceview->invalidate();
}
