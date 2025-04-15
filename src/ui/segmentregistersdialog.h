#pragma once

#include <QComboBox>
#include <QDialog>
#include <QTreeView>
#include <QVBoxLayout>

namespace ui {

struct SegmentRegistersDialog {
    QComboBox* cbregisters;
    QTreeView* tvsregs;

    explicit SegmentRegistersDialog(QDialog* self) {
        // self->setAttribute(Qt::WA_DeleteOnClose);
        self->setWindowTitle("Segment Registers");
        // self->setModal(true);
        self->resize(800, 600);

        this->cbregisters = new QComboBox();
        this->tvsregs = new QTreeView();

        auto* vbox = new QVBoxLayout(self);
        vbox->addWidget(this->cbregisters);
        vbox->addWidget(this->tvsregs, 1);
    }
};

} // namespace ui
