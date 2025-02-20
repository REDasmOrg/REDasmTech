#pragma once

#include "../ui/regionsdialog.h"
#include <QDialog>
#include <QSet>
#include <redasm/redasm.h>

class RegionsDialog: public QDialog {
    Q_OBJECT

public:
    explicit RegionsDialog(QWidget* parent = nullptr);

private:
    void populate_registers();

private:
    QSet<int> m_registers;
    const Map(RDProgramRegion) m_regions;
    ui::RegionsDialog m_ui;
};
