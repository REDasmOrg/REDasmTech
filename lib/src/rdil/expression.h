#pragma once

#include "../utils/object.h"
#include <forward_list>
#include <redasm/rdil.h>

namespace redasm::rdil {

struct ILExpr {
    RDILOp op{RDIL_INVALID};

    union {
        const ILExpr *n1{nullptr}, *u, *cond;
    };

    union {
        const ILExpr *n2{nullptr}, *dst, *l, *t;
    };

    union {
        const ILExpr *n3{nullptr}, *src, *r, *f;
    };

    RD_PRIVATE_RDIL_VALUE_FIELDS
};

class ILExprPool: public Object {
private:
    using Pool = std::forward_list<ILExpr>;

public:
    const ILExpr* expr_unknown();
    const ILExpr* expr_nop();
    const ILExpr* expr_pop(const ILExpr* e);
    const ILExpr* expr_push(const ILExpr* e);
    const ILExpr* expr_reg(int reg);
    const ILExpr* expr_sym(const char* sym);
    const ILExpr* expr_cnst(u64 value);
    const ILExpr* expr_var(RDAddress address);
    const ILExpr* expr_goto(const ILExpr* e);
    const ILExpr* expr_call(const ILExpr* e);
    const ILExpr* expr_ret(const ILExpr* e);
    const ILExpr* expr_mem(const ILExpr* e);
    const ILExpr* expr_not(const ILExpr* e);
    const ILExpr* expr_add(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_sub(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_mul(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_div(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_mod(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_and(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_or(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_xor(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_lsl(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_lsr(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_asl(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_asr(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_rol(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_ror(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_copy(const ILExpr* dst, const ILExpr* src);
    const ILExpr* expr_if(const ILExpr* cond, const ILExpr* t, const ILExpr* f);
    const ILExpr* expr_eq(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_ne(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_lt(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_le(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_gt(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_ge(const ILExpr* l, const ILExpr* r);
    const ILExpr* expr_int(const ILExpr* e);

protected:
    const ILExpr* check(const ILExpr* e);

private:
    ILExpr* expr_ds(RDILOp op, const ILExpr* dst, const ILExpr* src);
    ILExpr* expr_lr(RDILOp op, const ILExpr* l, const ILExpr* r);
    ILExpr* expr_u(RDILOp op, const ILExpr* u);
    ILExpr* expr(RDILOp op);

private:
    Pool m_pool;
};

} // namespace redasm::rdil
