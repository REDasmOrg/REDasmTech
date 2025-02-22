#pragma once

#include "../models/regionsmodel.h"
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

private Q_SLOTS:
    void populate_regions();

private:
    ui::RegionsDialog m_ui;
    QSet<int> m_registers;
    const Map(RDProgramRegion) m_regions;
    RegionsModel* m_regionsmodel;
};
