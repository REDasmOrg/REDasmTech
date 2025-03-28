#pragma once

#include "../ui/loaderdialog.h"
#include <QDialog>
#include <redasm/redasm.h>

class LoaderDialog: public QDialog {
    Q_OBJECT

public:
    explicit LoaderDialog(RDBuffer* buffer, QWidget* parent = nullptr);

private:
    void select_processor(const RDProcessorPlugin* proc);
    void populate_processors();
    void populate_loglevels() const;

public Q_SLOTS:
    void on_loader_changed(int currentrow);
    void on_processor_changed(int currentrow);
    void accept() override;
    void reject() override;

public:
    bool selected{false};

private:
    ui::LoaderDialog m_ui;
    const RDTestResultSlice* m_testresult{nullptr};
    const RDProcessorPluginSlice* m_processors{nullptr};
};
