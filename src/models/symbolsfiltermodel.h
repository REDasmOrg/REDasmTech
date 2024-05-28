#pragma once

#include "symbolsmodel.h"
#include <QSortFilterProxyModel>
#include <redasm/redasm.h>

class SymbolsModel;

class SymbolsFilterModel: public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit SymbolsFilterModel(QObject* parent = nullptr);
    explicit SymbolsFilterModel(usize filter, QObject* parent = nullptr);
    [[nodiscard]] RDAddress address(const QModelIndex& index) const;

    inline void set_type_filter(usize s) {
        m_typefilter = s;
        this->invalidate();
    }

    [[nodiscard]] inline const SymbolsModel* symbols_model() const {
        return qobject_cast<const SymbolsModel*>(this->sourceModel());
    }

    inline SymbolsModel* symbols_model() {
        return qobject_cast<SymbolsModel*>(this->sourceModel());
    }

protected:
    [[nodiscard]] bool filterAcceptsRow(int source_row,
                                        const QModelIndex&) const override;

private:
    usize m_typefilter{SYMBOL_INVALID};
};
