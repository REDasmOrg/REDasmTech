#include "expression.h"

namespace redasm::rdil {

ILExpression* ILExpressionTree::expr_unknown() {
    return this->expr(RDIL_UNKNOWN);
}
ILExpression* ILExpressionTree::expr_nop() { return this->expr(RDIL_NOP); }

ILExpression* ILExpressionTree::expr_pop(const ILExpression* e) {
    return this->expr_u(RDIL_POP, e);
}

ILExpression* ILExpressionTree::expr_push(const ILExpression* e) {
    return this->expr_u(RDIL_PUSH, e);
}

ILExpression* ILExpressionTree::expr_reg(const char* reg) {
    ILExpression* expr = this->expr(RDIL_CNST);
    expr->reg = reg;
    return expr;
}

ILExpression* ILExpressionTree::expr_cnst(u64 value) {
    ILExpression* expr = this->expr(RDIL_CNST);
    expr->u_value = value;
    return expr;
}

ILExpression* ILExpressionTree::expr_var(RDAddress address) {
    ILExpression* expr = this->expr(RDIL_VAR);
    expr->address = address;
    return expr;
}

ILExpression* ILExpressionTree::expr_goto(const ILExpression* e) {
    return this->expr_u(RDIL_GOTO, e);
}

ILExpression* ILExpressionTree::expr_call(const ILExpression* e) {
    return this->expr_u(RDIL_CALL, e);
}

ILExpression* ILExpressionTree::expr_ret(const ILExpression* e) {
    return this->expr_u(RDIL_RET, e);
}

ILExpression* ILExpressionTree::expr_mem(const ILExpression* e) {
    return this->expr_u(RDIL_MEM, e);
}

ILExpression* ILExpressionTree::expr_not(const ILExpression* e) {
    return this->expr_u(RDIL_NOT, e);
}

ILExpression* ILExpressionTree::expr_add(const ILExpression* l,
                                         const ILExpression* r) {
    return this->expr_lr(RDIL_ADD, l, r);
}

ILExpression* ILExpressionTree::expr_sub(const ILExpression* l,
                                         const ILExpression* r) {
    return this->expr_lr(RDIL_SUB, l, r);
}

ILExpression* ILExpressionTree::expr_mul(const ILExpression* l,
                                         const ILExpression* r) {
    return this->expr_lr(RDIL_MUL, l, r);
}

ILExpression* ILExpressionTree::expr_div(const ILExpression* l,
                                         const ILExpression* r) {
    return this->expr_lr(RDIL_DIV, l, r);
}

ILExpression* ILExpressionTree::expr_mod(const ILExpression* l,
                                         const ILExpression* r) {
    return this->expr_lr(RDIL_MOD, l, r);
}

ILExpression* ILExpressionTree::expr_and(const ILExpression* l,
                                         const ILExpression* r) {
    return this->expr_lr(RDIL_AND, l, r);
}

ILExpression* ILExpressionTree::expr_or(const ILExpression* l,
                                        const ILExpression* r) {
    return this->expr_lr(RDIL_OR, l, r);
}

ILExpression* ILExpressionTree::expr_xor(const ILExpression* l,
                                         const ILExpression* r) {
    return this->expr_lr(RDIL_XOR, l, r);
}

ILExpression* ILExpressionTree::expr_lsl(const ILExpression* l,
                                         const ILExpression* r) {
    return this->expr_lr(RDIL_LSL, l, r);
}

ILExpression* ILExpressionTree::expr_lsr(const ILExpression* l,
                                         const ILExpression* r) {
    return this->expr_lr(RDIL_LSR, l, r);
}

ILExpression* ILExpressionTree::expr_asl(const ILExpression* l,
                                         const ILExpression* r) {
    return this->expr_lr(RDIL_ASL, l, r);
}

ILExpression* ILExpressionTree::expr_asr(const ILExpression* l,
                                         const ILExpression* r) {
    return this->expr_lr(RDIL_ASR, l, r);
}

ILExpression* ILExpressionTree::expr_rol(const ILExpression* l,
                                         const ILExpression* r) {
    return this->expr_lr(RDIL_ROL, l, r);
}

ILExpression* ILExpressionTree::expr_ror(const ILExpression* l,
                                         const ILExpression* r) {
    return this->expr_lr(RDIL_ROR, l, r);
}

ILExpression* ILExpressionTree::expr_copy(const ILExpression* dst,
                                          const ILExpression* src) {
    return this->expr_ds(RDIL_ROR, dst, src);
}

ILExpression* ILExpressionTree::expr_if(const ILExpression* cond,
                                        const ILExpression* t,
                                        const ILExpression* f) {
    ILExpression* expr = this->expr(RDIL_IF);
    expr->cond = this->check(cond);
    expr->t = this->check(t);
    expr->f = this->check(f);
    return expr;
}

ILExpression* ILExpressionTree::expr_eq(const ILExpression* l,
                                        const ILExpression* r) {
    return this->expr_lr(RDIL_EQ, l, r);
}

ILExpression* ILExpressionTree::expr_ne(const ILExpression* l,
                                        const ILExpression* r) {
    return this->expr_lr(RDIL_NE, l, r);
}

ILExpression* ILExpressionTree::expr_lt(const ILExpression* l,
                                        const ILExpression* r) {
    return this->expr_lr(RDIL_LT, l, r);
}

ILExpression* ILExpressionTree::expr_le(const ILExpression* l,
                                        const ILExpression* r) {
    return this->expr_lr(RDIL_LE, l, r);
}

ILExpression* ILExpressionTree::expr_gt(const ILExpression* l,
                                        const ILExpression* r) {
    return this->expr_lr(RDIL_GT, l, r);
}

ILExpression* ILExpressionTree::expr_ge(const ILExpression* l,
                                        const ILExpression* r) {
    return this->expr_lr(RDIL_GE, l, r);
}

ILExpression* ILExpressionTree::expr_int(const ILExpression* e) {
    return this->expr_u(RDIL_INT, e);
}

const ILExpression* ILExpressionTree::check(const ILExpression* e) {
    return e ? e : this->expr_unknown();
}

ILExpression* ILExpressionTree::expr_ds(RDILOp op, const ILExpression* dst,
                                        const ILExpression* src) {
    ILExpression* expr = this->expr(op);
    expr->dst = this->check(dst);
    expr->src = this->check(src);
    return expr;
}

ILExpression* ILExpressionTree::expr_lr(RDILOp op, const ILExpression* l,
                                        const ILExpression* r) {
    ILExpression* expr = this->expr(op);
    expr->l = this->check(l);
    expr->r = this->check(r);
    return expr;
}

ILExpression* ILExpressionTree::expr_u(RDILOp op, const ILExpression* u) {
    ILExpression* expr = this->expr(op);
    expr->u = this->check(u);
    return expr;
}

ILExpression* ILExpressionTree::expr(RDILOp op) {
    auto& expr = m_pool.emplace_front();
    expr.op = op;
    return &expr;
}

} // namespace redasm::rdil
