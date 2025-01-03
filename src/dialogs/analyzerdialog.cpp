#include "analyzerdialog.h"

AnalyzerDialog::AnalyzerDialog(QWidget* parent): QDialog{parent}, m_ui{this} {
    m_analyzersmodel = new QStandardItemModel(m_ui.tblanalyzers);
    m_ui.tblanalyzers->setModel(m_analyzersmodel);

    this->get_analyzers();
    this->set_details_visible(false);

    connect(m_analyzersmodel, &QStandardItemModel::itemChanged, this,
            [&](QStandardItem* item) {
                rdanalyzer_select(&m_analyzers[item->index().row()],
                                  (item->checkState() == Qt::Checked));
            });

    connect(m_ui.chkshowdetails, &QCheckBox::checkStateChanged, this,
            [&](Qt::CheckState state) {
                this->set_details_visible(state == Qt::Checked);
            });

    connect(m_ui.pbselectall, &QPushButton::clicked, this,
            [&]() { this->select_analyzers(true); });
    connect(m_ui.pbunselectall, &QPushButton::clicked, this,
            [&]() { this->select_analyzers(false); });
    connect(m_ui.pbrestoredefaults, &QPushButton::clicked, this,
            &AnalyzerDialog::get_analyzers);
}

void AnalyzerDialog::set_details_visible(bool b) {
    m_ui.tblanalyzers->setColumnHidden(m_analyzersmodel->columnCount() - 1, !b);
    m_ui.tblanalyzers->horizontalHeader()->setVisible(b);
}

void AnalyzerDialog::select_analyzers(bool select) {
    for(int i = 0; i < m_analyzersmodel->rowCount(); i++) {
        auto* item = m_analyzersmodel->item(i);
        item->setCheckState(select ? Qt::Checked : Qt::Unchecked);
        rdanalyzer_select(&m_analyzers[i], select);
    }
}

void AnalyzerDialog::get_analyzers() {
    m_analyzersmodel->clear();
    m_analyzersmodel->setHorizontalHeaderLabels({"Name", "Order"});

    m_nanalyzers = rd_getanalyzers(&m_analyzers);

    for(usize i = 0; i < m_nanalyzers; i++) {
        QString name = m_analyzers[i].name;
        QString order = QString::number(m_analyzers[i].order, 16);
        auto* nameitem = new QStandardItem(name);
        auto* orderitem = new QStandardItem(order);

        nameitem->setCheckable(true);

        nameitem->setCheckState(
            m_analyzers[i].flags & ANA_SELECTED ? Qt::Checked : Qt::Unchecked);

        m_analyzersmodel->appendRow({nameitem, orderitem});
    }

    m_ui.tblanalyzers->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::Stretch);
    m_ui.tblanalyzers->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::ResizeToContents);

    this->set_details_visible(m_ui.chkshowdetails->checkState());
}
