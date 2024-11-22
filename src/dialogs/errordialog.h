#pragma once

#include "../ui/errordialog.h"

class ErrorDialog: public QDialog {
    Q_OBJECT

private:
    explicit ErrorDialog(QWidget* parent = nullptr);

public:
    static void show_error(const QString& title, const QString& msg);

private:
    ui::ErrorDialog m_ui;
};
