#pragma once

#include "../models/symbolsfiltermodel.h"
#include "../ui/contextview.h"

class ContextView: public QWidget {
    Q_OBJECT

public:
    explicit ContextView(QWidget* parent = nullptr);
    ~ContextView() override;
    [[nodiscard]] inline bool active() const { return m_active; }
    void tick(const RDEngineStatus* s);

    inline void jump_to(RDAddress address) { // NOLINT
        return m_ui.splitview->jump_to(address);
    }

    inline void invalidate() { // NOLINT
        return m_ui.splitview->invalidate();
    }

    [[nodiscard]] inline RDSurface* handle() const {
        return m_ui.splitview->handle();
    };

private:
    ui::ContextView m_ui;
    SymbolsFilterModel* m_functionsmodel;
    bool m_active{true};
};
