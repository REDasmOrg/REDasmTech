#include "symbolsmodel.h"
#include "../themeprovider.h"
#include "../utils.h"

namespace {

QString mangled_string(const char* s) {
    QString res;

    while(*s) {
        switch(*s) {
            case '\r': res += "\\r"; break;
            case '\n': res += "\\n"; break;
            case '\t': res += "\\t"; break;
            default: res += *s; break;
        }

        s++;
    }

    return res;
}

} // namespace

SymbolsModel::SymbolsModel(bool autoalign, QObject* parent)
    : QAbstractListModel{parent}, m_autoalign{autoalign} {
    m_nsymbols = rdlisting_getsymbolslength();
    m_colsymbol = tr("Symbol");
}

RDAddress SymbolsModel::address(const QModelIndex& index) const {
    RDSymbol symbol;
    if(rdlisting_getsymbol(index.row(), &symbol)) return symbol.address;
    qFatal("Cannot get symbol address");
    return {};
}

void SymbolsModel::resync() {
    this->beginResetModel();
    m_nsymbols = rdlisting_getsymbolslength();
    this->endResetModel();
}

QVariant SymbolsModel::data(const QModelIndex& index, int role) const {
    RDSymbol symbol;
    if(!rdlisting_getsymbol(index.row(), &symbol)) return {};

    if(role == Qt::DisplayRole) {
        switch(index.column()) {
            case 0: return utils::to_hex_addr(symbol.address);
            case 1: return this->get_symbol_type(symbol.type);

            case 2: {
                if(symbol.type == SYMBOL_STRING)
                    return mangled_string(symbol.value);
                return symbol.value;
            }

            default: break;
        }
    }
    else if(m_autoalign && role == Qt::TextAlignmentRole) {
        if(index.column() == 0)
            return QVariant{Qt::AlignRight | Qt::AlignVCenter};
        if(index.column() == 1) return Qt::AlignCenter;
        return Qt::AlignLeft;
    }
    else if(role == Qt::ForegroundRole) {
        if(m_highlightaddress && index.column() == 0)
            return themeprovider::color(THEME_ADDRESS);
        if(m_highlightsymbol && index.column() == 2)
            return themeprovider::color(symbol.theme);
    }
    else if(role == Qt::UserRole)
        return QVariant::fromValue(symbol.type);

    return {};
}

QVariant SymbolsModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const {
    if(orientation == Qt::Vertical || role != Qt::DisplayRole) return {};

    switch(section) {
        case 0: return tr("Address");
        case 1: return tr("Type");
        case 2: return m_colsymbol;
        default: break;
    }

    return {};
}

int SymbolsModel::columnCount(const QModelIndex&) const { return 3; }
int SymbolsModel::rowCount(const QModelIndex&) const { return m_nsymbols; }

QString SymbolsModel::get_symbol_type(usize t) const {
    switch(t) {
        case SYMBOL_SEGMENT: return "SEGMENT";
        case SYMBOL_FUNCTION: return "FUNCTION";
        case SYMBOL_STRING: return "STRING";
        case SYMBOL_TYPE: return "TYPE";
        default: break;
    }

    return {};
}
