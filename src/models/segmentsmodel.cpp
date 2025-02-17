#include "segmentsmodel.h"
#include "../utils.h"

namespace {

QString get_segment_type(const RDSegment& seg) {
    QString s;
    if(seg.perm & SP_R) s.append("R");
    if(seg.perm & SP_W) s.append("W");
    if(seg.perm & SP_X) s.append("X");
    return s;
}

} // namespace

SegmentsModel::SegmentsModel(QObject* parent): QAbstractListModel{parent} {
    m_nsegments = rd_getsegments(&m_segments);
}

size_t SegmentsModel::address(const QModelIndex& index) const {
    return m_segments[index.row()].start;
}

QVariant SegmentsModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::DisplayRole) {
        const RDSegment& seg = m_segments[index.row()];

        switch(index.column()) {
            case 0: return m_segments[index.row()].name;
            case 1: return utils::to_hex_addr(seg.start, &seg);
            case 2: return utils::to_hex_addr(seg.end, &seg);
            case 3: return utils::to_hex_addr(seg.end - seg.start, &seg);
            case 4: return QString::number(seg.bits);
            case 5: return get_segment_type(seg);
            default: break;
        }
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
        case 3: return "Size";
        case 4: return "Bits";
        case 5: return "Perm";
        default: break;
    }

    return {};
}

int SegmentsModel::columnCount(const QModelIndex&) const { return 6; }
int SegmentsModel::rowCount(const QModelIndex&) const { return m_nsegments; }
