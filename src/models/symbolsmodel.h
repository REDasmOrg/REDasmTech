#pragma once

#include <QAbstractListModel>
#include <redasm/redasm.h>

class SymbolsModel: public QAbstractListModel {
    Q_OBJECT

public:
    explicit SymbolsModel(bool autoalign = true, QObject* parent = nullptr);
    [[nodiscard]] RDAddress address(const QModelIndex& index) const;
    void set_symbol_column_text(const QString& s) { m_colsymbol = s; }
    void set_highlight_address(bool b) { m_highlightaddress = b; }
    void set_highlight_symbol(bool b) { m_highlightsymbol = b; }
    void resync();

public:
    [[nodiscard]] QVariant data(const QModelIndex& index,
                                int role) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                      int role) const override;
    [[nodiscard]] int columnCount(const QModelIndex&) const override;
    [[nodiscard]] int rowCount(const QModelIndex&) const override;

private:
    [[nodiscard]] QString get_symbol_type(usize t) const;

private:
    QString m_colsymbol;
    usize m_nsymbols;
    bool m_autoalign;
    bool m_highlightaddress{false}, m_highlightsymbol{false};
};
