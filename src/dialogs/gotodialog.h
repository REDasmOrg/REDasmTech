#pragma once

#include "../models/symbolsmodel.h"
#include "../ui/gotodialog.h"
#include <QDialog>
#include <QSortFilterProxyModel>
#include <redasm/redasm.h>

class GotoDialog: public QDialog {
    Q_OBJECT

public:
    explicit GotoDialog(QWidget* parent = nullptr);

private Q_SLOTS:
    void on_item_selected(const QModelIndex& index);
    void on_goto_clicked();

private:
    void validate_and_filter_entry();
    bool check_valid_address();

public:
    RDAddress address{0};

private:
    ui::GotoDialog m_ui;
    bool m_validaddress{false};
    SymbolsModel* m_symbolsmodel;
    QSortFilterProxyModel* m_filtermodel;
};
