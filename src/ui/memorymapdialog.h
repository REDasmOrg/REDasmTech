#pragma once

#include <QBoxLayout>
#include <QDialog>
#include <QVBoxLayout>
#include <qhexview/qhexview.h>

namespace ui {

struct MemoryMapDialog {
    QHexView* hexview;

    explicit MemoryMapDialog(QDialog* self) {
        self->setAttribute(Qt::WA_DeleteOnClose);
        self->setWindowTitle("Memory Map");
        self->setModal(true);
        self->resize(800, 600);

        auto* vboxlayout = new QVBoxLayout(self);
        this->hexview = new QHexView();
        this->hexview->setReadOnly(true);
        vboxlayout->addWidget(this->hexview);
    }
};

} // namespace ui
