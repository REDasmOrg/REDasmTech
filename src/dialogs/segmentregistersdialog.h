#pragma once

#include "../models/segmentregistersmodel.h"
#include "../ui/segmentregistersdialog.h"
#include <QDialog>
#include <QSet>
#include <redasm/redasm.h>

class SegmentRegistersDialog: public QDialog {
    Q_OBJECT

public:
    explicit SegmentRegistersDialog(QWidget* parent = nullptr);

    [[nodiscard]] const SegmentRegisterModel* model() const {
        return m_segmentregistersmodel;
    }

private:
    void populate_registers();

private Q_SLOTS:
    void update_register();

Q_SIGNALS:
    void double_clicked(const QModelIndex& index);

private:
    ui::SegmentRegistersDialog m_ui;
    // const Map(RDSRegItem) m_segmentregisters;
    SegmentRegisterModel* m_segmentregistersmodel;
};
