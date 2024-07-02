#pragma once

#include "expression.h"
#include <vector>

namespace redasm::rdil {

class ILFunction: public rdil::ILExpressionTree {
public:
    using ExpressionList = std::vector<std::pair<MIndex, const ILExpression*>>;

public:
    void insert(usize idx, const ILExpression* e);
    void append(const ILExpression* e);
    const ILExpression* first() const;
    const ILExpression* last() const;
    const ILExpression* expression(usize idx) const;

    inline bool empty() const { return m_expressions.empty(); }
    inline usize size() const { return m_expressions.size(); }

public:
    inline ExpressionList::iterator begin() { return m_expressions.begin(); }
    inline ExpressionList::iterator end() { return m_expressions.end(); }

    inline ExpressionList::const_iterator begin() const {
        return m_expressions.begin();
    }

    inline ExpressionList::const_iterator end() const {
        return m_expressions.end();
    }

private:
    ExpressionList m_expressions;
};

} // namespace redasm::rdil
