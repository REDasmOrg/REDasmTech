#include "regionsdialog.h"

RegionsDialog::RegionsDialog(QWidget* parent): QDialog{parent}, m_ui{this} {
    m_regions = rd_getregions();
    map_foreach(const RDProgramRegion, r, m_regions) m_registers.insert(r->key);
    this->populate_registers();

    m_regionsmodel = new RegionsModel(this);
    m_ui.tvregions->setModel(m_regionsmodel);

    connect(m_ui.cbregisters, &QComboBox::currentIndexChanged, this,
            &RegionsDialog::populate_regions);

    this->populate_regions();
}

void RegionsDialog::populate_registers() {
    m_ui.cbregisters->clear();

    if(m_registers.empty()) {
        m_ui.cbregisters->setVisible(!m_registers.empty());
        return;
    }

    for(int regid : m_registers)
        m_ui.cbregisters->addItem(rd_getregistername(regid), regid);
}

void RegionsDialog::populate_regions() {
    m_regionsmodel->set_register(m_ui.cbregisters->currentData().toInt());
}
