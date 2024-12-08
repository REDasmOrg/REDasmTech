#pragma once

#include <QAbstractListModel>
#include <QStringList>
#include <redasm/redasm.h>

class ProblemsModel: public QAbstractListModel {
    Q_OBJECT

public:
    explicit ProblemsModel(QObject* parent = nullptr);
    [[nodiscard]] RDAddress address(const QModelIndex& index) const;

public:
    [[nodiscard]] int rowCount(const QModelIndex&) const override;
    [[nodiscard]] int columnCount(const QModelIndex&) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index,
                                int role) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                      int role) const override;

private:
    const RDProblem* m_problems{nullptr};
    usize m_nproblems{};
};
