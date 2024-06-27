#pragma once

#include "../models/symbolsfiltermodel.h"
#include "../ui/contextview.h"

class ContextView: public QWidget {
    Q_OBJECT

public:
    explicit ContextView(QWidget* parent = nullptr);
    ~ContextView() override;
    [[nodiscard]] RDSurface* handle() const;
    void tick(const RDEngineStatus* s);
    void jump_to(RDAddress address);
    void invalidate();

private:
    ui::ContextView m_ui;
    SymbolsFilterModel* m_functionsmodel;
};
