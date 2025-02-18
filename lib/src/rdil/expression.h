#pragma once

#include <forward_list>
#include <redasm/rdil.h>

namespace redasm::rdil {

class ILExprPool {
private:
    using Pool = std::forward_list<RDILExpr>;

public:
    const RDILExpr* expr_unknown();
    const RDILExpr* expr_nop();
    const RDILExpr* expr_pop(const RDILExpr* e);
    const RDILExpr* expr_push(const RDILExpr* e);
    const RDILExpr* expr_reg(int reg);
    const RDILExpr* expr_sym(const char* sym);
    const RDILExpr* expr_cnst(u64 value);
    const RDILExpr* expr_var(RDAddress address);
    const RDILExpr* expr_goto(const RDILExpr* e);
    const RDILExpr* expr_call(const RDILExpr* e);
    const RDILExpr* expr_ret(const RDILExpr* e);
    const RDILExpr* expr_mem(const RDILExpr* e);
    const RDILExpr* expr_not(const RDILExpr* e);
    const RDILExpr* expr_add(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_sub(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_mul(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_div(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_mod(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_and(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_or(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_xor(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_lsl(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_lsr(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_asl(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_asr(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_rol(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_ror(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_copy(const RDILExpr* dst, const RDILExpr* src);
    const RDILExpr* expr_if(const RDILExpr* cond, const RDILExpr* t,
                            const RDILExpr* f);
    const RDILExpr* expr_eq(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_ne(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_lt(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_le(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_gt(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_ge(const RDILExpr* l, const RDILExpr* r);
    const RDILExpr* expr_int(const RDILExpr* e);

protected:
    const RDILExpr* check(const RDILExpr* e);

private:
    RDILExpr* expr_ds(RDILOp op, const RDILExpr* dst, const RDILExpr* src);
    RDILExpr* expr_lr(RDILOp op, const RDILExpr* l, const RDILExpr* r);
    RDILExpr* expr_u(RDILOp op, const RDILExpr* u);
    RDILExpr* expr(RDILOp op);

public:
    RDAddress currentaddress{};

private:
    Pool m_pool;
};

} // namespace redasm::rdil
