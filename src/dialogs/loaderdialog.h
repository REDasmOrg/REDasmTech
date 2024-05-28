#pragma once

#include "../ui/loaderdialog.h"
#include <QDialog>
#include <redasm/redasm.h>

class LoaderDialog: public QDialog {
    Q_OBJECT

public:
    explicit LoaderDialog(RDBuffer* buffer, QWidget* parent = nullptr);

    [[nodiscard]] inline bool can_disassemble() const {
        return m_ui.chkdisassemble->isChecked();
    }

private:
    void select_processor(const RDProcessor* processor);
    void populate_processors();
    void populate_loglevels() const;

public Q_SLOTS:
    void on_loader_changed(int currentrow);
    void accept() override;
    void reject() override;

public:
    RDContext* context{nullptr};

private:
    ui::LoaderDialog m_ui;
    const RDTestResult* m_testresult{nullptr};
    const RDProcessor* m_processors{nullptr};
    usize m_nprocessors{0};
};
