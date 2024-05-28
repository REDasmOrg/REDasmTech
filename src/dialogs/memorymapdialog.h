#pragma once

#include "../ui/memorymapdialog.h"
#include <QDialog>

class MemoryMapDialog: public QDialog {
    Q_OBJECT

public:
    explicit MemoryMapDialog(QWidget* parent = nullptr);

private:
    ui::MemoryMapDialog m_ui;
};
