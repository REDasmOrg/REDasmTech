#include "expressionlist.h"

namespace redasm::rdil {

void ILExprList::append(const RDILExpr* e) {
    e = this->check(e);
    m_expressions.push_back(e);
}

const RDILExpr* ILExprList::first() const {
    if(m_expressions.empty()) return nullptr;
    return m_expressions.front();
}

const RDILExpr* ILExprList::last() const {
    if(m_expressions.empty()) return nullptr;
    return m_expressions.back();
}

const RDILExpr* ILExprList::at(usize idx) const {
    if(idx >= m_expressions.size()) return nullptr;
    return m_expressions[idx];
}

} // namespace redasm::rdil
