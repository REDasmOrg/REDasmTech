#pragma once

#include <QAbstractListModel>
#include <redasm/redasm.h>

class SegmentsModel: public QAbstractListModel {
    Q_OBJECT

public:
    explicit SegmentsModel(QObject* parent = nullptr);
    [[nodiscard]] size_t address(const QModelIndex& index) const;

public:
    [[nodiscard]] QVariant data(const QModelIndex& index,
                                int role) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                      int role) const override;
    [[nodiscard]] int columnCount(const QModelIndex&) const override;
    [[nodiscard]] int rowCount(const QModelIndex&) const override;

private:
    const RDSegment* m_segments{nullptr};
    usize m_nsegments{};
};
