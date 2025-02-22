#pragma once

#include <QAbstractListModel>
#include <redasm/redasm.h>

class RegionsModel: public QAbstractListModel {
    Q_OBJECT

public:
    explicit RegionsModel(QObject* parent = nullptr);
    void set_register(int reg);

public:
    [[nodiscard]] int rowCount(const QModelIndex&) const override;
    [[nodiscard]] int columnCount(const QModelIndex&) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index,
                                int role) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                      int role) const override;

private:
    RDAddress m_address;
    const Map(RDProgramRegion) m_regionsbyreg;
    const Vect(RDRegion) m_regions;
};
