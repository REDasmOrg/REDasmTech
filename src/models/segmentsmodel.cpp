#include "segmentsmodel.h"
#include "../themeprovider.h"

SegmentsModel::SegmentsModel(QObject* parent): QAbstractListModel{parent} {
    m_nsegments = rd_getsegments(&m_segments);
}

size_t SegmentsModel::address(const QModelIndex& index) const {
    return m_segments[index.row()].address;
}

QVariant SegmentsModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::DisplayRole) {
        auto address = m_segments[index.row()].address;
        auto endaddress = m_segments[index.row()].endaddress;
        auto offset = m_segments[index.row()].offset;
        auto endoffset = m_segments[index.row()].endoffset;

        switch(index.column()) {
            case 0: return m_segments[index.row()].name;
            case 1: return rd_tohex(address);
            case 2: return rd_tohex(endaddress);
            case 3: return rd_tohex(endaddress - address);
            case 4: return rd_tohex(offset);
            case 5: return rd_tohex(endoffset);
            case 6: return rd_tohex(endoffset - offset);
            default: break;
        }
    }
    else if(role == Qt::ForegroundRole) {
        if(index.column() == 0)
            return themeprovider::color(THEME_LABEL);
        if(index.column() < 7)
            return themeprovider::color(THEME_ADDRESS);
        if(index.column() == 7)
            return themeprovider::color(THEME_DATA);
    }
    else if(role == Qt::TextAlignmentRole) {
        if(index.column() == 0)
            return QVariant{Qt::AlignRight | Qt::AlignVCenter};
        return Qt::AlignCenter;
    }

    return {};
}

QVariant SegmentsModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const {
    if(orientation == Qt::Vertical || role != Qt::DisplayRole)
        return {};

    switch(section) {
        case 0: return tr("Name");
        case 1: return tr("Start Address");
        case 2: return tr("End Address");
        case 3: return tr("Address Size");
        case 4: return tr("Offset");
        case 5: return tr("End Offset");
        case 6: return tr("Offset Size");
        default: break;
    }

    return {};
}

int SegmentsModel::columnCount(const QModelIndex&) const { return 7; }

int SegmentsModel::rowCount(const QModelIndex&) const { return m_nsegments; }
