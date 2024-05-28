#pragma once

#include "../ui/flcdialog.h"
#include <QDialog>

class FLCDialog: public QDialog {
    Q_OBJECT

public:
    explicit FLCDialog(QWidget* parent = nullptr);

private Q_SLOTS:
    void on_address_changed(const QString& s);
    void on_offset_changed(const QString& s);

private:
    ui::FLCDialog m_ui;
};
