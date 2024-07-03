#include "expression.h"

namespace redasm::rdil {

const ILExpression* ILExpressionPool::expr_unknown() {
    static ILExpression expr{};
    expr.op = RDIL_UNKNOWN;
    return &expr;
}

const ILExpression* ILExpressionPool::expr_nop() {
    static ILExpression expr{};
    expr.op = RDIL_NOP;
    return &expr;
}

const ILExpression* ILExpressionPool::expr_pop(const ILExpression* e) {
    return this->expr_u(RDIL_POP, e);
}

const ILExpression* ILExpressionPool::expr_push(const ILExpression* e) {
    return this->expr_u(RDIL_PUSH, e);
}

const ILExpression* ILExpressionPool::expr_reg(const char* reg) {
    ILExpression* expr = this->expr(RDIL_CNST);
    expr->reg = reg;
    return expr;
}

const ILExpression* ILExpressionPool::expr_cnst(u64 value) {
    ILExpression* expr = this->expr(RDIL_CNST);
    expr->u_value = value;
    return expr;
}

const ILExpression* ILExpressionPool::expr_var(RDAddress address) {
    ILExpression* expr = this->expr(RDIL_VAR);
    expr->address = address;
    return expr;
}

const ILExpression* ILExpressionPool::expr_goto(const ILExpression* e) {
    return this->expr_u(RDIL_GOTO, e);
}

const ILExpression* ILExpressionPool::expr_call(const ILExpression* e) {
    return this->expr_u(RDIL_CALL, e);
}

const ILExpression* ILExpressionPool::expr_ret(const ILExpression* e) {
    return this->expr_u(RDIL_RET, e);
}

const ILExpression* ILExpressionPool::expr_mem(const ILExpression* e) {
    return this->expr_u(RDIL_MEM, e);
}

const ILExpression* ILExpressionPool::expr_not(const ILExpression* e) {
    return this->expr_u(RDIL_NOT, e);
}

const ILExpression* ILExpressionPool::expr_add(const ILExpression* l,
                                               const ILExpression* r) {
    return this->expr_lr(RDIL_ADD, l, r);
}

const ILExpression* ILExpressionPool::expr_sub(const ILExpression* l,
                                               const ILExpression* r) {
    return this->expr_lr(RDIL_SUB, l, r);
}

const ILExpression* ILExpressionPool::expr_mul(const ILExpression* l,
                                               const ILExpression* r) {
    return this->expr_lr(RDIL_MUL, l, r);
}

const ILExpression* ILExpressionPool::expr_div(const ILExpression* l,
                                               const ILExpression* r) {
    return this->expr_lr(RDIL_DIV, l, r);
}

const ILExpression* ILExpressionPool::expr_mod(const ILExpression* l,
                                               const ILExpression* r) {
    return this->expr_lr(RDIL_MOD, l, r);
}

const ILExpression* ILExpressionPool::expr_and(const ILExpression* l,
                                               const ILExpression* r) {
    return this->expr_lr(RDIL_AND, l, r);
}

const ILExpression* ILExpressionPool::expr_or(const ILExpression* l,
                                              const ILExpression* r) {
    return this->expr_lr(RDIL_OR, l, r);
}

const ILExpression* ILExpressionPool::expr_xor(const ILExpression* l,
                                               const ILExpression* r) {
    return this->expr_lr(RDIL_XOR, l, r);
}

const ILExpression* ILExpressionPool::expr_lsl(const ILExpression* l,
                                               const ILExpression* r) {
    return this->expr_lr(RDIL_LSL, l, r);
}

const ILExpression* ILExpressionPool::expr_lsr(const ILExpression* l,
                                               const ILExpression* r) {
    return this->expr_lr(RDIL_LSR, l, r);
}

const ILExpression* ILExpressionPool::expr_asl(const ILExpression* l,
                                               const ILExpression* r) {
    return this->expr_lr(RDIL_ASL, l, r);
}

const ILExpression* ILExpressionPool::expr_asr(const ILExpression* l,
                                               const ILExpression* r) {
    return this->expr_lr(RDIL_ASR, l, r);
}

const ILExpression* ILExpressionPool::expr_rol(const ILExpression* l,
                                               const ILExpression* r) {
    return this->expr_lr(RDIL_ROL, l, r);
}

const ILExpression* ILExpressionPool::expr_ror(const ILExpression* l,
                                               const ILExpression* r) {
    return this->expr_lr(RDIL_ROR, l, r);
}

const ILExpression* ILExpressionPool::expr_copy(const ILExpression* dst,
                                                const ILExpression* src) {
    return this->expr_ds(RDIL_ROR, dst, src);
}

const ILExpression* ILExpressionPool::expr_if(const ILExpression* cond,
                                              const ILExpression* t,
                                              const ILExpression* f) {
    ILExpression* expr = this->expr(RDIL_IF);
    expr->cond = this->check(cond);
    expr->t = this->check(t);
    expr->f = this->check(f);
    return expr;
}

const ILExpression* ILExpressionPool::expr_eq(const ILExpression* l,
                                              const ILExpression* r) {
    return this->expr_lr(RDIL_EQ, l, r);
}

const ILExpression* ILExpressionPool::expr_ne(const ILExpression* l,
                                              const ILExpression* r) {
    return this->expr_lr(RDIL_NE, l, r);
}

const ILExpression* ILExpressionPool::expr_lt(const ILExpression* l,
                                              const ILExpression* r) {
    return this->expr_lr(RDIL_LT, l, r);
}

const ILExpression* ILExpressionPool::expr_le(const ILExpression* l,
                                              const ILExpression* r) {
    return this->expr_lr(RDIL_LE, l, r);
}

const ILExpression* ILExpressionPool::expr_gt(const ILExpression* l,
                                              const ILExpression* r) {
    return this->expr_lr(RDIL_GT, l, r);
}

const ILExpression* ILExpressionPool::expr_ge(const ILExpression* l,
                                              const ILExpression* r) {
    return this->expr_lr(RDIL_GE, l, r);
}

const ILExpression* ILExpressionPool::expr_int(const ILExpression* e) {
    return this->expr_u(RDIL_INT, e);
}

const ILExpression* ILExpressionPool::check(const ILExpression* e) {
    return e ? e : this->expr_unknown();
}

ILExpression* ILExpressionPool::expr_ds(RDILOp op, const ILExpression* dst,
                                        const ILExpression* src) {
    ILExpression* expr = this->expr(op);
    expr->dst = this->check(dst);
    expr->src = this->check(src);
    return expr;
}

ILExpression* ILExpressionPool::expr_lr(RDILOp op, const ILExpression* l,
                                        const ILExpression* r) {
    ILExpression* expr = this->expr(op);
    expr->l = this->check(l);
    expr->r = this->check(r);
    return expr;
}

ILExpression* ILExpressionPool::expr_u(RDILOp op, const ILExpression* u) {
    ILExpression* expr = this->expr(op);
    expr->u = this->check(u);
    return expr;
}

ILExpression* ILExpressionPool::expr(RDILOp op) {
    auto& expr = m_pool.emplace_front();
    expr.op = op;
    return &expr;
}

} // namespace redasm::rdil
