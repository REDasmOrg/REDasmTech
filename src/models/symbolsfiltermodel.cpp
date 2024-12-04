#include "symbolsfiltermodel.h"
#include <QRegularExpression>

SymbolsFilterModel::SymbolsFilterModel(bool autoalign, QObject* parent)
    : QSortFilterProxyModel{parent} {
    this->setSourceModel(new SymbolsModel(autoalign, this));
    this->setFilterKeyColumn(2);
}

SymbolsFilterModel::SymbolsFilterModel(usize filter, bool autoalign,
                                       QObject* parent)
    : SymbolsFilterModel{autoalign, parent} {
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
    QModelIndex index = this->sourceModel()->index(source_row, 2);
    QString s = index.data().toString();

    if(!s.contains(this->filterRegularExpression()))
        return false;

    if(m_typefilter != SYMBOL_INVALID)
        return m_typefilter == index.data(Qt::UserRole).toUInt();

    return true;
}
