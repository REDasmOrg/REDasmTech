#include "regionsdialog.h"

RegionsDialog::RegionsDialog(QWidget* parent): QDialog{parent}, m_ui{this} {
    m_regions = rd_getregions();
    map_foreach(const RDProgramRegion, r, m_regions) m_registers.insert(r->key);
    this->populate_registers();
}

void RegionsDialog::populate_registers() {
    m_ui.cbregisters->clear();

    if(m_registers.empty()) {
        m_ui.cbregisters->setVisible(!m_registers.empty());
        return;
    }

    for(int regid : m_registers) {
        m_ui.cbregisters->addItem(QString("reg%1").arg(regid), regid);
    }
}
