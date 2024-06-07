#pragma once

#include "../models/symbolsfiltermodel.h"
#include "../ui/contextview.h"

class ContextView: public QWidget {
    Q_OBJECT

public:
    explicit ContextView(QWidget* parent = nullptr);
    ~ContextView() override;
    [[nodiscard]] SurfaceView* surface_view() const { return m_ui.surfaceview; }
    void tick(const RDEngineStatus* s);

private:
    ui::ContextView m_ui;
    SymbolsFilterModel* m_functionsmodel;
};
