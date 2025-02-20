#pragma once

#include <QComboBox>
#include <QDialog>
#include <QTreeView>
#include <QVBoxLayout>

namespace ui {

struct RegionsDialog {
    QComboBox* cbregisters;
    QTreeView* tvregions;

    explicit RegionsDialog(QDialog* self) {
        self->setAttribute(Qt::WA_DeleteOnClose);
        self->setWindowTitle("Regions");
        self->setModal(true);
        self->resize(800, 600);

        this->cbregisters = new QComboBox();
        this->tvregions = new QTreeView();

        auto* vbox = new QVBoxLayout(self);
        vbox->addWidget(this->cbregisters);
        vbox->addWidget(this->tvregions, 1);
    }
};

} // namespace ui
