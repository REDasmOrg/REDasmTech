#include "regionsmodel.h"
#include "../utils.h"

RegionsModel::RegionsModel(QObject* parent)
    : QAbstractListModel{parent}, m_regionsbyreg{rd_getregions()},
      m_regions{nullptr} {}

void RegionsModel::set_register(int reg) {
    this->beginResetModel();

    if(const Vect(RDRegion)* r = map_get(RDProgramRegion, m_regionsbyreg, reg);
       r) {
        m_regions = *r;
    }
    else
        m_regions = nullptr;

    this->endResetModel();
}

RDAddress RegionsModel::address(const QModelIndex& index) const {
    if(static_cast<usize>(index.row()) < vect_length(m_regions))
        return m_regions[index.row()].start;

    qFatal("Cannot get region");
    return {};
}

int RegionsModel::rowCount(const QModelIndex&) const {
    if(!m_regions) return 0;
    return vect_length(m_regions);
}

int RegionsModel::columnCount(const QModelIndex&) const { return 3; }

QVariant RegionsModel::data(const QModelIndex& index, int role) const {
    if(m_regions && role == Qt::DisplayRole) {
        switch(index.column()) {
            case 0: return utils::to_hex_addr(m_regions[index.row()].start);
            case 1: return utils::to_hex_addr(m_regions[index.row()].end);
            case 2: return rd_tohex(m_regions[index.row()].value);
            default: break;
        }
    }

    return {};
}

QVariant RegionsModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const {
    if(orientation == Qt::Vertical || role != Qt::DisplayRole) return {};

    switch(section) {
        case 0: return "Start";
        case 1: return "End";
        case 2: return "Value";
        default: break;
    }

    return {};
}
