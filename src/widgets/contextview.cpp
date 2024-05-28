#include "contextview.h"
#include "../mainwindow.h"
#include "../utils.h"

ContextView::ContextView(QWidget* parent): QWidget{parent}, m_ui{this} {
    m_functionsmodel = new SymbolsFilterModel(SYMBOL_FUNCTION, this);
    m_functionsmodel->symbols_model()->set_highlight_address(true);
    m_ui.tvfunctions->setModel(m_functionsmodel);
    m_ui.tvfunctions->header()->hideSection(1);
    m_ui.tvfunctions->header()->setSectionResizeMode(
        0, QHeaderView::ResizeToContents);

    connect(m_ui.tvfunctions, &QTreeView::doubleClicked, this,
            [&](const QModelIndex& index) {
                ContextView* ctxview = utils::mainwindow->context_view();
                if(ctxview) {
                    RDAddress address = m_functionsmodel->address(index);
                    ctxview->surface_view()->jump_to(address);
                    ctxview->surface_view()->setFocus();
                }
            });
}

ContextView::~ContextView() { rd_destroy(); }
