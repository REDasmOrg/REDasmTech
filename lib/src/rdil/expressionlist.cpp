#include "expressionlist.h"

namespace redasm::rdil {

void ILExpressionList::append(const ILExpression* e) {
    e = this->check(e);
    m_expressions.emplace_back(this->currentindex, e);
}

const ILExpression* ILExpressionList::first() const {
    if(m_expressions.empty())
        return nullptr;

    return m_expressions.front().second;
}

const ILExpression* ILExpressionList::last() const {
    if(m_expressions.empty())
        return nullptr;

    return m_expressions.back().second;
}

const ILExpression* ILExpressionList::at(usize idx) const {
    if(idx >= m_expressions.size())
        return nullptr;

    return m_expressions[idx].second;
}

} // namespace redasm::rdil
