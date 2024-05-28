#pragma once

#include <QBoxLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QTreeWidget>

namespace ui {

struct TableDialog {
    QDialogButtonBox* buttonbox;
    QLineEdit* lesearch;
    QTreeView* tvtable;

    explicit TableDialog(QDialog* self) {
        self->setAttribute(Qt::WA_DeleteOnClose);
        self->resize(800, 600);

        this->tvtable = new QTreeView();
        this->lesearch = new QLineEdit();
        this->lesearch->setPlaceholderText("Search…");

        this->buttonbox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                               QDialogButtonBox::Cancel);
        QObject::connect(this->buttonbox, &QDialogButtonBox::accepted, self,
                         &QDialog::accept);
        QObject::connect(this->buttonbox, &QDialogButtonBox::rejected, self,
                         &QDialog::reject);

        auto* vboxlayout = new QVBoxLayout(self);
        vboxlayout->addWidget(this->lesearch);
        vboxlayout->addWidget(this->tvtable);
        vboxlayout->addWidget(this->buttonbox);
    }
};

} // namespace ui
