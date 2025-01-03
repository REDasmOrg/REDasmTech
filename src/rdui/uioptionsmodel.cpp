#include "uioptionsmodel.h"

namespace qtui {

UIOptionsModel::UIOptionsModel(RDUIOptions* options, size_t c, QObject* parent)
    : QAbstractListModel(parent), m_options(options), m_count(c) {}

void UIOptionsModel::uncheck_all() {
    this->beginResetModel();

    for(usize i = 0; i < m_count; i++)
        m_options[i].selected = false;

    this->endResetModel();
}

Qt::ItemFlags UIOptionsModel::flags(const QModelIndex& index) const {
    return QAbstractListModel::flags(index) | Qt::ItemIsUserCheckable;
}

int UIOptionsModel::rowCount(const QModelIndex&) const {
    return static_cast<int>(m_count);
}

QVariant UIOptionsModel::data(const QModelIndex& index, int role) const {
    const auto& option = m_options[index.row()];

    if(role == Qt::DisplayRole) return option.text;
    if(role == Qt::CheckStateRole)
        return option.selected ? Qt::Checked : Qt::Unchecked;

    return {};
}

bool UIOptionsModel::setData(const QModelIndex& index, const QVariant& value,
                             int role) {
    if(role == Qt::CheckStateRole) {
        auto& option = m_options[index.row()];
        option.selected = value == Qt::Checked;
        Q_EMIT dataChanged(index, index);
        return true;
    }

    return QAbstractListModel::setData(index, value, role);
}

} // namespace qtui
