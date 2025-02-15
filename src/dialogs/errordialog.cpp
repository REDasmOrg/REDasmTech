#include "errordialog.h"
#include "../mainwindow.h"
#include "../utils.h"
#include <QTimer>

namespace {

ErrorDialog* errdlg = nullptr;

}

ErrorDialog::ErrorDialog(QWidget* parent): QDialog{parent}, m_ui{this} {
    connect(m_ui.pbquit, &QPushButton::clicked, this, [&]() { qFatal(); });
    connect(m_ui.pbcontinue, &QPushButton::clicked, this, &ErrorDialog::close);
}

void ErrorDialog::show_error(const QString& title, const QString& msg) {
    if(!errdlg) errdlg = new ErrorDialog(utils::mainwindow);

    errdlg->m_ui.pteerror->setPlainText(msg);
    errdlg->setWindowTitle(title);
    errdlg->show();

    // Bring error on top
    QTimer::singleShot(500, []() { errdlg->activateWindow(); });
}
