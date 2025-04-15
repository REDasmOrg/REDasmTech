#include "segmentregistersdialog.h"
#include <QHeaderView>

SegmentRegistersDialog::SegmentRegistersDialog(QWidget* parent)
    : QDialog{parent}, m_ui{this} {
    m_segmentregistersmodel = new SegmentRegisterModel(this);
    m_ui.tvsregs->setModel(m_segmentregistersmodel);
    m_ui.tvsregs->header()->setStretchLastSection(true);
    m_ui.tvsregs->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(m_ui.tvsregs, &QTreeView::doubleClicked, this,
            &SegmentRegistersDialog::double_clicked);

    connect(m_ui.cbregisters, &QComboBox::currentIndexChanged, this,
            &SegmentRegistersDialog::update_register);
}

void SegmentRegistersDialog::showEvent(QShowEvent* e) {
    QDialog::showEvent(e);
    m_segmentregisters = rd_getsegmentregisters();

    QSet<int> sregs;
    const RDSRegTree* tree;
    hmap_foreach(tree, m_segmentregisters, RDSRegTree, hnode)
        sregs.insert(tree->sreg);

    m_sregs.assign(sregs.begin(), sregs.end());
    this->populate_registers();
    this->update_register();
}

void SegmentRegistersDialog::populate_registers() {
    m_ui.cbregisters->clear();

    if(m_sregs.empty()) {
        m_ui.cbregisters->setVisible(!m_sregs.empty());
        return;
    }

    for(int regid : m_sregs)
        m_ui.cbregisters->addItem(rd_getregistername(regid), regid);
}

void SegmentRegistersDialog::update_register() {
    m_segmentregistersmodel->set_register(
        m_ui.cbregisters->currentData().toInt());
}
