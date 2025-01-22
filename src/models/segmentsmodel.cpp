#include "segmentsmodel.h"
#include "../themeprovider.h"

namespace {

QString get_segment_type(const RDSegment& seg) {
    QString s;

    if(seg.type & SEG_HASCODE) s.append("CODE");

    if(seg.type & SEG_HASDATA) {
        if(!s.isEmpty()) s.append(" | ");

        s.append("DATA");
    }

    return s;
}

} // namespace

SegmentsModel::SegmentsModel(QObject* parent): QAbstractListModel{parent} {
    m_nsegments = rd_getsegments(&m_segments);
}

size_t SegmentsModel::address(const QModelIndex& index) const {
    return m_segments[index.row()].startaddr;
}

QVariant SegmentsModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::DisplayRole) {
        auto address = m_segments[index.row()].startaddr;
        auto endaddress = m_segments[index.row()].endaddr;
        auto offset = m_segments[index.row()].startoff;
        auto endoffset = m_segments[index.row()].endoff;

        switch(index.column()) {
            case 0: return m_segments[index.row()].name;
            case 1: return rd_tohex(address);
            case 2: return rd_tohex(endaddress);
            case 3: return rd_tohex(endaddress - address);
            case 4: return rd_tohex(offset);
            case 5: return rd_tohex(endoffset);
            case 6: return rd_tohex(endoffset - offset);
            case 7: return get_segment_type(m_segments[index.row()]);
            default: break;
        }
    }
    else if(role == Qt::ForegroundRole) {
        if(index.column() == 0) return themeprovider::color(THEME_LABEL);
        if(index.column() < 7) return themeprovider::color(THEME_ADDRESS);
        if(index.column() == 7) return themeprovider::color(THEME_DATA);
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
    if(orientation == Qt::Vertical || role != Qt::DisplayRole) return {};

    switch(section) {
        case 0: return "Name";
        case 1: return "Start Address";
        case 2: return "End Address";
        case 3: return "Address Size";
        case 4: return "Offset";
        case 5: return "End Offset";
        case 6: return "Offset Size";
        case 7: return "Type";
        default: break;
    }

    return {};
}

int SegmentsModel::columnCount(const QModelIndex&) const { return 8; }
int SegmentsModel::rowCount(const QModelIndex&) const { return m_nsegments; }
