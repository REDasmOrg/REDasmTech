#pragma once

#include <QAbstractListModel>
#include <redasm/ui.h>

namespace qtui {

class UIOptionsModel: public QAbstractListModel {
    Q_OBJECT

public:
    UIOptionsModel(RDUIOptions* options, usize c, QObject* parent = nullptr);
    void uncheck_all();

public:
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    [[nodiscard]] int rowCount(const QModelIndex&) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index,
                                int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value,
                 int role) override;

private:
    RDUIOptions* m_options;
    usize m_count;
};
} // namespace qtui
