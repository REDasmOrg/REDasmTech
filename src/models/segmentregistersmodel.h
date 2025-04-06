#pragma once

#include <QAbstractListModel>
#include <redasm/redasm.h>

class SegmentRegisterModel: public QAbstractListModel {
    Q_OBJECT

public:
    explicit SegmentRegisterModel(QObject* parent = nullptr);
    [[nodiscard]] RDAddress address(const QModelIndex& index) const;
    void set_register(int reg);

public:
    [[nodiscard]] int rowCount(const QModelIndex&) const override;
    [[nodiscard]] int columnCount(const QModelIndex&) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index,
                                int role) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                      int role) const override;

private:
    const RDSRangeMap* m_segmentregs;
    QList<const RDSRange*> m_ranges;
};
