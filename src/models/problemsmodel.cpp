#include "problemsmodel.h"

ProblemsModel::ProblemsModel(QObject* parent): QAbstractListModel{parent} {
    m_nproblems = rd_getproblems(&m_problems);
}

QVariant ProblemsModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::DisplayRole) {
        switch(index.column()) {
            case 0: return rd_tohex(m_problems[index.row()].address);
            case 1: return QString::fromUtf8(m_problems[index.row()].problem);
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

QVariant ProblemsModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const {
    if(orientation == Qt::Vertical || role != Qt::DisplayRole)
        return {};

    switch(section) {
        case 0: return tr("Address");
        case 1: return tr("Problem");
        default: break;
    }

    return {};
}

int ProblemsModel::columnCount(const QModelIndex&) const { return 2; }
int ProblemsModel::rowCount(const QModelIndex&) const { return m_nproblems; }
