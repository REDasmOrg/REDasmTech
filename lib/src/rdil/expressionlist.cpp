#include "expressionlist.h"

namespace redasm::rdil {

void ILExprList::append(const RDILExpr* e) {
    e = this->check(e);
    m_expressions.emplace_back(this->currentaddress, e);
}

const RDILExpr* ILExprList::first() const {
    if(m_expressions.empty()) return nullptr;

    return m_expressions.front().second;
}

const RDILExpr* ILExprList::last() const {
    if(m_expressions.empty()) return nullptr;

    return m_expressions.back().second;
}

const RDILExpr* ILExprList::at(usize idx) const {
    if(idx >= m_expressions.size()) return nullptr;

    return m_expressions[idx].second;
}

} // namespace redasm::rdil
