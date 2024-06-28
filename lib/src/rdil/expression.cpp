#include "expression.h"

namespace redasm::rdil {

Expression* ExpressionTree::expr_unknown() { return this->expr(RDIL_UNKNOWN); }
Expression* ExpressionTree::expr_nop() { return this->expr(RDIL_NOP); }

Expression* ExpressionTree::expr_pop(const Expression* e) {
    return this->expr_u(RDIL_POP, e);
}

Expression* ExpressionTree::expr_push(const Expression* e) {
    return this->expr_u(RDIL_PUSH, e);
}

Expression* ExpressionTree::expr_reg(const char* reg) {
    Expression* expr = this->expr(RDIL_CNST);
    expr->reg = reg;
    return expr;
}

Expression* ExpressionTree::expr_cnst(u64 value) {
    Expression* expr = this->expr(RDIL_CNST);
    expr->u_value = value;
    return expr;
}

Expression* ExpressionTree::expr_var(RDAddress address) {
    Expression* expr = this->expr(RDIL_VAR);
    expr->address = address;
    return expr;
}

Expression* ExpressionTree::expr_goto(const Expression* e) {
    return this->expr_u(RDIL_GOTO, e);
}

Expression* ExpressionTree::expr_call(const Expression* e) {
    return this->expr_u(RDIL_CALL, e);
}

Expression* ExpressionTree::expr_ret(const Expression* e) {
    return this->expr_u(RDIL_RET, e);
}

Expression* ExpressionTree::expr_mem(const Expression* e) {
    return this->expr_u(RDIL_MEM, e);
}

Expression* ExpressionTree::expr_not(const Expression* e) {
    return this->expr_u(RDIL_NOT, e);
}

Expression* ExpressionTree::expr_add(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_ADD, l, r);
}

Expression* ExpressionTree::expr_sub(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_SUB, l, r);
}

Expression* ExpressionTree::expr_mul(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_MUL, l, r);
}

Expression* ExpressionTree::expr_div(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_DIV, l, r);
}

Expression* ExpressionTree::expr_mod(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_MOD, l, r);
}

Expression* ExpressionTree::expr_and(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_AND, l, r);
}

Expression* ExpressionTree::expr_or(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_OR, l, r);
}

Expression* ExpressionTree::expr_xor(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_XOR, l, r);
}

Expression* ExpressionTree::expr_lsl(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_LSL, l, r);
}

Expression* ExpressionTree::expr_lsr(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_LSR, l, r);
}

Expression* ExpressionTree::expr_asl(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_ASL, l, r);
}

Expression* ExpressionTree::expr_asr(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_ASR, l, r);
}

Expression* ExpressionTree::expr_rol(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_ROL, l, r);
}

Expression* ExpressionTree::expr_ror(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_ROR, l, r);
}

Expression* ExpressionTree::expr_copy(const Expression* dst,
                                      const Expression* src) {
    return this->expr_ds(RDIL_ROR, dst, src);
}

Expression* ExpressionTree::expr_if(const Expression* cond, const Expression* t,
                                    const Expression* f) {
    Expression* expr = this->expr(RDIL_IF);
    expr->cond = this->check(cond);
    expr->t = this->check(t);
    expr->f = this->check(f);
    return expr;
}

Expression* ExpressionTree::expr_eq(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_EQ, l, r);
}

Expression* ExpressionTree::expr_ne(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_NE, l, r);
}

Expression* ExpressionTree::expr_lt(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_LT, l, r);
}

Expression* ExpressionTree::expr_le(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_LE, l, r);
}

Expression* ExpressionTree::expr_gt(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_GT, l, r);
}

Expression* ExpressionTree::expr_ge(const Expression* l, const Expression* r) {
    return this->expr_lr(RDIL_GE, l, r);
}

Expression* ExpressionTree::expr_int(const Expression* e) {
    return this->expr_u(RDIL_INT, e);
}

const Expression* ExpressionTree::check(const Expression* e) {
    return e ? e : this->expr_unknown();
}

Expression* ExpressionTree::expr_ds(RDILOpCodes op, const Expression* dst,
                                    const Expression* src) {
    Expression* expr = this->expr(op);
    expr->dst = this->check(dst);
    expr->src = this->check(src);
    return expr;
}

Expression* ExpressionTree::expr_lr(RDILOpCodes op, const Expression* l,
                                    const Expression* r) {
    Expression* expr = this->expr(op);
    expr->l = this->check(l);
    expr->r = this->check(r);
    return expr;
}

Expression* ExpressionTree::expr_u(RDILOpCodes op, const Expression* u) {
    Expression* expr = this->expr(op);
    expr->u = this->check(u);
    return expr;
}

Expression* ExpressionTree::expr(RDILOpCodes op) {
    auto& expr = m_pool.emplace_front();
    expr.op = op;
    return &expr;
}

} // namespace redasm::rdil
