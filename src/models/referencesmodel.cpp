#include "referencesmodel.h"
#include "../utils.h"

namespace {

QString reftype_tostring(const RDRef& r) {
    switch(r.type) {
        case DR_READ: return "Read";
        case DR_WRITE: return "Write";
        case DR_ADDRESS: return "Address";
        case CR_CALL: return "Call";
        case CR_JUMP: return "Jump";
        default: break;
    }

    return {};
}

QString ref_getdirection(RDAddress address, const RDRef& r) {
    if(r.address > address) return "Down";
    if(r.address < address) return "Up";
    return "---";
}

} // namespace

ReferencesModel::ReferencesModel(RDAddress address, QObject* parent)
    : QAbstractListModel{parent}, m_address{address} {
    m_nrefs = rd_getrefsto(address, &m_refs);
}

RDAddress ReferencesModel::address(const QModelIndex& index) const {
    if(static_cast<usize>(index.row()) < m_nrefs)
        return m_refs[index.row()].address;

    qFatal("Cannot get reference");
    return {};
}

QVariant ReferencesModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::DisplayRole) {
        switch(index.column()) {
            case 0: return utils::to_hex_addr(m_refs[index.row()].address);
            case 1: return reftype_tostring(m_refs[index.row()]);
            case 2: return ref_getdirection(m_address, m_refs[index.row()]);
            case 3: return rd_rendertext(m_refs[index.row()].address);
            default: break;
        }
    }
    else if(role == Qt::TextAlignmentRole) {
        if(index.column() == 0) return Qt::AlignRight;
        if(index.column() == 3) return Qt::AlignLeft;
    }

    return {};
}

QVariant ReferencesModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const {
    if(orientation == Qt::Vertical || role != Qt::DisplayRole) return {};

    switch(section) {
        case 0: return "Address";
        case 1: return "Type";
        case 2: return "Direction";
        case 3: return "Reference";
        default: break;
    }

    return {};
}

int ReferencesModel::columnCount(const QModelIndex&) const { return 4; }
int ReferencesModel::rowCount(const QModelIndex&) const { return m_nrefs; }
