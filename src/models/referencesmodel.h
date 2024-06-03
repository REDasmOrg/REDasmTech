#pragma once

#include <QAbstractListModel>
#include <QStringList>
#include <redasm/redasm.h>

class ReferencesModel: public QAbstractListModel {
    Q_OBJECT

public:
    explicit ReferencesModel(RDAddress address, QObject* parent = nullptr);
    [[nodiscard]] RDAddress address(const QModelIndex& index) const;
    void update();

public:
    [[nodiscard]] int rowCount(const QModelIndex&) const override;
    [[nodiscard]] int columnCount(const QModelIndex&) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index,
                                int role) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                      int role) const override;

private:
    RDAddress m_address;
    const RDRef* m_refs;
    usize m_nrefs;
};
