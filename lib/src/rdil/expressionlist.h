#pragma once

#include "expression.h"
#include <vector>

namespace redasm::rdil {

class ILExpressionList: public ILExpressionPool {
public:
    using Container = std::vector<std::pair<MIndex, const ILExpression*>>;

public:
    void append(const ILExpression* e);
    const ILExpression* first() const;
    const ILExpression* last() const;
    const ILExpression* at(usize idx) const;

    inline bool empty() const { return m_expressions.empty(); }
    inline usize size() const { return m_expressions.size(); }

public:
    inline Container::iterator begin() { return m_expressions.begin(); }
    inline Container::iterator end() { return m_expressions.end(); }

    inline Container::const_iterator begin() const {
        return m_expressions.begin();
    }

    inline Container::const_iterator end() const { return m_expressions.end(); }

private:
    Container m_expressions;

public:
    MIndex currentindex{};
};

} // namespace redasm::rdil
