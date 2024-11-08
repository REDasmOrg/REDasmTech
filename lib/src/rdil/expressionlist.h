#pragma once

#include "expression.h"
#include <vector>

namespace redasm::rdil {

class ILExprList: public ILExprPool {
public:
    using Container = std::vector<std::pair<MIndex, const ILExpr*>>;

public:
    void append(const ILExpr* e);
    const ILExpr* first() const;
    const ILExpr* last() const;
    const ILExpr* at(usize idx) const;
    bool empty() const { return m_expressions.empty(); }
    usize size() const { return m_expressions.size(); }
    void clear() { m_expressions.clear(); }

public:
    Container::iterator begin() { return m_expressions.begin(); }
    Container::iterator end() { return m_expressions.end(); }
    Container::const_iterator begin() const { return m_expressions.begin(); }
    Container::const_iterator end() const { return m_expressions.end(); }

private:
    Container m_expressions;

public:
    MIndex currentindex{};
};

} // namespace redasm::rdil
