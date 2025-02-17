#include "exportsmodel.h"
#include "../utils.h"

ExportsModel::ExportsModel(QObject* parent): QAbstractListModel{parent} {
    m_nexports = rdlisting_getexportslength();
}

RDAddress ExportsModel::address(const QModelIndex& index) const {
    RDSymbol symbol;
    if(rdlisting_getexport(index.row(), &symbol)) return symbol.address;
    qFatal("Cannot get export address");
    return {};
}

QVariant ExportsModel::data(const QModelIndex& index, int role) const {
    RDSymbol symbol;
    if(!rdlisting_getexport(index.row(), &symbol)) return {};

    if(role == Qt::DisplayRole) {
        switch(index.column()) {
            case 0: return utils::to_hex_addr(symbol.address);
            case 1: return symbol.value;
            default: break;
        }
    }
    else if(role == Qt::TextAlignmentRole) {
        if(index.column() == 0)
            return QVariant{Qt::AlignRight | Qt::AlignVCenter};
        return Qt::AlignLeft;
    }

    return {};
}

QVariant ExportsModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const {
    if(orientation == Qt::Vertical || role != Qt::DisplayRole) return {};

    switch(section) {
        case 0: return tr("Address");
        case 1: return tr("Name");
        default: break;
    }

    return {};
}

int ExportsModel::columnCount(const QModelIndex&) const { return 2; }
int ExportsModel::rowCount(const QModelIndex&) const { return m_nexports; }
