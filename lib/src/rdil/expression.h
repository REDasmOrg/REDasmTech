#pragma once

#include "../utils/object.h"
#include <forward_list>
#include <redasm/rdil.h>

namespace redasm::rdil {

struct Expression {
    RDILOpCodes op{RDIL_INVALID};

    union {
        const Expression *n1{nullptr}, *u, *cond;
    };

    union {
        const Expression *n2{nullptr}, *dst, *l, *t;
    };

    union {
        const Expression *n3{nullptr}, *src, *r, *f;
    };

    RD_PRIVATE_RDIL_VALUE_FIELDS
};

class ExpressionTree: public Object {
public:
    Expression* expr_unknown();
    Expression* expr_nop();
    Expression* expr_pop(const Expression* e);
    Expression* expr_push(const Expression* e);
    Expression* expr_reg(const char* reg);
    Expression* expr_cnst(u64 value);
    Expression* expr_var(RDAddress address);
    Expression* expr_goto(const Expression* e);
    Expression* expr_call(const Expression* e);
    Expression* expr_ret(const Expression* e);
    Expression* expr_mem(const Expression* e);
    Expression* expr_not(const Expression* e);
    Expression* expr_add(const Expression* l, const Expression* r);
    Expression* expr_sub(const Expression* l, const Expression* r);
    Expression* expr_mul(const Expression* l, const Expression* r);
    Expression* expr_div(const Expression* l, const Expression* r);
    Expression* expr_mod(const Expression* l, const Expression* r);
    Expression* expr_and(const Expression* l, const Expression* r);
    Expression* expr_or(const Expression* l, const Expression* r);
    Expression* expr_xor(const Expression* l, const Expression* r);
    Expression* expr_lsl(const Expression* l, const Expression* r);
    Expression* expr_lsr(const Expression* l, const Expression* r);
    Expression* expr_asl(const Expression* l, const Expression* r);
    Expression* expr_asr(const Expression* l, const Expression* r);
    Expression* expr_rol(const Expression* l, const Expression* r);
    Expression* expr_ror(const Expression* l, const Expression* r);
    Expression* expr_copy(const Expression* dst, const Expression* src);
    Expression* expr_if(const Expression* cond, const Expression* t,
                        const Expression* f);
    Expression* expr_eq(const Expression* l, const Expression* r);
    Expression* expr_ne(const Expression* l, const Expression* r);
    Expression* expr_lt(const Expression* l, const Expression* r);
    Expression* expr_le(const Expression* l, const Expression* r);
    Expression* expr_gt(const Expression* l, const Expression* r);
    Expression* expr_ge(const Expression* l, const Expression* r);
    Expression* expr_int(const Expression* e);

private:
    const Expression* check(const Expression* e);
    Expression* expr_ds(RDILOpCodes op, const Expression* dst,
                        const Expression* src);
    Expression* expr_lr(RDILOpCodes op, const Expression* l,
                        const Expression* r);
    Expression* expr_u(RDILOpCodes op, const Expression* u);
    Expression* expr(RDILOpCodes op);

private:
    std::forward_list<Expression> m_pool;
};

} // namespace redasm::rdil
