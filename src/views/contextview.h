#pragma once

#include "../models/symbolsfiltermodel.h"
#include "../ui/contextview.h"

class ContextView: public QWidget {
    Q_OBJECT

public:
    explicit ContextView(QWidget* parent = nullptr);
    ~ContextView() override;
    [[nodiscard]] bool active() const { return m_active; }
    void tick(const RDEngineStatus* s);
    [[nodiscard]] bool has_rdil() const { return m_ui.splitview->has_rdil(); }
    void set_rdil(bool b) { m_ui.splitview->set_rdil(b); } // NOLINT

    void jump_to(RDAddress address) { // NOLINT
        m_ui.splitview->jump_to(address);
    }

    void invalidate() { // NOLINT
        m_ui.splitview->invalidate();
    }

    [[nodiscard]] RDSurface* handle() const {
        return m_ui.splitview->handle();
    };

private:
    ui::ContextView m_ui;
    SymbolsFilterModel* m_functionsmodel;
    bool m_active{true};
};
