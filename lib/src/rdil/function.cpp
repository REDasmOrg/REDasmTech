#include "function.h"

namespace redasm::rdil {

void ILFunction::insert(usize idx, const ILExpression* e) {}

void ILFunction::append(const ILExpression* e) {}

const ILExpression* ILFunction::first() const {
    if(m_expressions.empty())
        return nullptr;

    return m_expressions.front().second;
}

const ILExpression* ILFunction::last() const {
    if(m_expressions.empty())
        return nullptr;

    return m_expressions.back().second;
}

const ILExpression* ILFunction::expression(usize idx) const {
    if(idx >= m_expressions.size())
        return nullptr;

    return m_expressions[idx].second;
}

} // namespace redasm::rdil
