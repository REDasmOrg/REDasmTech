#include "segmentregistersmodel.h"
#include "../utils.h"

SegmentRegisterModel::SegmentRegisterModel(QObject* parent)
    : QAbstractListModel{parent}, m_segmentregs{rd_getsegmentregisters()} {}

void SegmentRegisterModel::set_register(int reg) {
    this->beginResetModel();
    m_ranges.clear();

    const RDSRegTree* tree;
    hmap_get(tree, m_segmentregs, RDSRegTree, hnode, ct_inttoptr(reg),
             tree->sreg == reg);

    if(tree) {
        const RDSRange* it;
        rbtree_foreach(it, &tree->root, RDSRange, rbnode)
            m_ranges.push_back(it);
    }

    this->endResetModel();
}

RDAddress SegmentRegisterModel::address(const QModelIndex& index) const {
    if(index.row() < m_ranges.size()) return m_ranges[index.row()]->start;
    qFatal("Cannot get segment register range");
    return {};
}

int SegmentRegisterModel::rowCount(const QModelIndex&) const {
    if(m_ranges.empty()) return 0;
    return m_ranges.size();
}

int SegmentRegisterModel::columnCount(const QModelIndex&) const { return 3; }

QVariant SegmentRegisterModel::data(const QModelIndex& index, int role) const {
    if(!m_ranges.empty() && role == Qt::DisplayRole) {
        switch(index.column()) {
            case 0: return utils::to_hex_addr(m_ranges[index.row()]->start);
            case 1: return utils::to_hex_addr(m_ranges[index.row()]->end);
            case 2: return rd_tohex(m_ranges[index.row()]->value);
            default: break;
        }
    }

    return {};
}

QVariant SegmentRegisterModel::headerData(int section,
                                          Qt::Orientation orientation,
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
