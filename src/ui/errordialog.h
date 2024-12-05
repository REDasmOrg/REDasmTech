#pragma once

#include <QDialog>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace ui {

struct ErrorDialog {
    QPlainTextEdit* pteerror;
    QPushButton *pbquit, *pbcontinue;

    explicit ErrorDialog(QDialog* self) {
        self->resize(640, 430);
        self->setWindowFlag(Qt::WindowStaysOnTopHint);
        self->setAttribute(Qt::WA_DeleteOnClose);

        this->pteerror = new QPlainTextEdit(self);
        this->pteerror->setWordWrapMode(QTextOption::NoWrap);
        this->pteerror->setUndoRedoEnabled(false);
        this->pteerror->setReadOnly(true);

        this->pbquit = new QPushButton("Quit");
        this->pbcontinue = new QPushButton("Continue");

        auto* hbox = new QHBoxLayout();
        hbox->setContentsMargins(0, 0, 0, 0);
        hbox->addStretch();
        hbox->addWidget(this->pbquit);
        hbox->addWidget(this->pbcontinue);

        auto* vbox = new QVBoxLayout(self);
        vbox->addWidget(this->pteerror);
        vbox->addLayout(hbox);
    }
};

} // namespace ui
