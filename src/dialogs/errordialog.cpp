#include "errordialog.h"
#include "../utils.h"
#include <QTimer>

ErrorDialog::ErrorDialog(QWidget* parent): QDialog{parent}, m_ui{this} {
    connect(m_ui.pbquit, &QPushButton::clicked, this, [&]() { qFatal(); });
    connect(m_ui.pbcontinue, &QPushButton::clicked, this, &ErrorDialog::close);
}

void ErrorDialog::show_error(const QString& title, const QString& msg) {
    static ErrorDialog errdlg;

    errdlg.m_ui.pteerror->setPlainText(msg);
    errdlg.setWindowTitle(title);
    errdlg.show();

    // Bring error on top
    QTimer::singleShot(500, []() { errdlg.activateWindow(); });
}
