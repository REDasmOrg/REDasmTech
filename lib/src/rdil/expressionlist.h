#pragma once

#include "expression.h"
#include <redasm/rdil.h>
#include <vector>

namespace redasm::rdil {

class ILExprList: public ILExprPool {
public:
    using Container = std::vector<std::pair<MIndex, const RDILExpr*>>;

public:
    void append(const RDILExpr* e);
    const RDILExpr* first() const;
    const RDILExpr* last() const;
    const RDILExpr* at(usize idx) const;
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
