#include "symbolsfiltermodel.h"
#include <QRegularExpression>

SymbolsFilterModel::SymbolsFilterModel(QObject* parent)
    : QSortFilterProxyModel{parent} {
    this->setSourceModel(new SymbolsModel(this));
    this->setFilterKeyColumn(1);
}

SymbolsFilterModel::SymbolsFilterModel(usize filter, QObject* parent)
    : SymbolsFilterModel{parent} {
    this->set_type_filter(filter);
}

RDAddress SymbolsFilterModel::address(const QModelIndex& index) const {
    return this->symbols_model()->address(this->mapToSource(index));
}

void SymbolsFilterModel::resync() {
    this->symbols_model()->resync();
    this->invalidate();
}

bool SymbolsFilterModel::filterAcceptsRow(int source_row,
                                          const QModelIndex&) const {
    QModelIndex index = this->sourceModel()->index(source_row, 1);
    QString s = index.data().toString();

    if(!s.contains(this->filterRegularExpression()))
        return false;

    if(m_typefilter != SYMBOL_INVALID)
        return m_typefilter == index.data(Qt::UserRole).toUInt();

    return true;
}
