#include "problemsmodel.h"
#include "../utils.h"

ProblemsModel::ProblemsModel(QObject* parent): QAbstractListModel{parent} {
    m_problems = rd_getproblems();
}

RDAddress ProblemsModel::address(const QModelIndex& index) const {
    if(static_cast<usize>(index.row()) < vect_length(m_problems))
        return m_problems[index.row()].address;

    qFatal("Cannot get problem");
    return {};
}

QVariant ProblemsModel::data(const QModelIndex& index, int role) const {
    if(!m_problems) return {};

    if(role == Qt::DisplayRole) {
        switch(index.column()) {
            case 0: return utils::to_hex_addr(m_problems[index.row()].address);
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
    if(orientation == Qt::Vertical || role != Qt::DisplayRole) return {};

    switch(section) {
        case 0: return tr("Address");
        case 1: return tr("Problem");
        default: break;
    }

    return {};
}

int ProblemsModel::columnCount(const QModelIndex&) const { return 2; }

int ProblemsModel::rowCount(const QModelIndex&) const {
    return vect_length(m_problems);
}
