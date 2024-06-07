#include "contextview.h"

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
                m_ui.surfaceview->jump_to(address);
                m_ui.surfaceview->setFocus();
            });
}

ContextView::~ContextView() { rd_destroy(); }

void ContextView::tick(const RDEngineStatus* s) {
    if(s->stepscurrent == STEP_EMULATE)
        return;

    m_functionsmodel->resync();
    m_ui.surfaceview->invalidate();

    if(s->stepscurrent == STEP_DONE)
        m_ui.surfaceview->jump_to_ep();
}
