#include "expressionlist.h"

namespace redasm::rdil {

void ILExprList::append(const ILExpr* e) {
    e = this->check(e);
    m_expressions.emplace_back(this->currentindex, e);
}

const ILExpr* ILExprList::first() const {
    if(m_expressions.empty())
        return nullptr;

    return m_expressions.front().second;
}

const ILExpr* ILExprList::last() const {
    if(m_expressions.empty())
        return nullptr;

    return m_expressions.back().second;
}

const ILExpr* ILExprList::at(usize idx) const {
    if(idx >= m_expressions.size())
        return nullptr;

    return m_expressions[idx].second;
}

} // namespace redasm::rdil
