#pragma once

#include "../utils/object.h"
#include <forward_list>
#include <redasm/rdil.h>

namespace redasm::rdil {

struct ILExpression {
    RDILOp op{RDIL_INVALID};

    union {
        const ILExpression *n1{nullptr}, *u, *cond;
    };

    union {
        const ILExpression *n2{nullptr}, *dst, *l, *t;
    };

    union {
        const ILExpression *n3{nullptr}, *src, *r, *f;
    };

    RD_PRIVATE_RDIL_VALUE_FIELDS
};

class ILExpressionPool: public Object {
private:
    using Pool = std::forward_list<ILExpression>;

public:
    const ILExpression* expr_unknown();
    const ILExpression* expr_nop();
    const ILExpression* expr_pop(const ILExpression* e);
    const ILExpression* expr_push(const ILExpression* e);
    const ILExpression* expr_reg(const char* reg);
    const ILExpression* expr_cnst(u64 value);
    const ILExpression* expr_var(RDAddress address);
    const ILExpression* expr_goto(const ILExpression* e);
    const ILExpression* expr_call(const ILExpression* e);
    const ILExpression* expr_ret(const ILExpression* e);
    const ILExpression* expr_mem(const ILExpression* e);
    const ILExpression* expr_not(const ILExpression* e);
    const ILExpression* expr_add(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_sub(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_mul(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_div(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_mod(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_and(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_or(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_xor(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_lsl(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_lsr(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_asl(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_asr(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_rol(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_ror(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_copy(const ILExpression* dst,
                                  const ILExpression* src);
    const ILExpression* expr_if(const ILExpression* cond, const ILExpression* t,
                                const ILExpression* f);
    const ILExpression* expr_eq(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_ne(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_lt(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_le(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_gt(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_ge(const ILExpression* l, const ILExpression* r);
    const ILExpression* expr_int(const ILExpression* e);

protected:
    const ILExpression* check(const ILExpression* e);

private:
    ILExpression* expr_ds(RDILOp op, const ILExpression* dst,
                          const ILExpression* src);
    ILExpression* expr_lr(RDILOp op, const ILExpression* l,
                          const ILExpression* r);
    ILExpression* expr_u(RDILOp op, const ILExpression* u);
    ILExpression* expr(RDILOp op);

private:
    Pool m_pool;
};

} // namespace redasm::rdil
