#include "expression.h"

namespace redasm::rdil {

const ILExpr* ILExprPool::expr_unknown() {
    static ILExpr expr{};
    expr.op = RDIL_UNKNOWN;
    return &expr;
}

const ILExpr* ILExprPool::expr_nop() {
    static ILExpr expr{};
    expr.op = RDIL_NOP;
    return &expr;
}

const ILExpr* ILExprPool::expr_pop(const ILExpr* e) {
    return this->expr_u(RDIL_POP, e);
}

const ILExpr* ILExprPool::expr_push(const ILExpr* e) {
    return this->expr_u(RDIL_PUSH, e);
}

const ILExpr* ILExprPool::expr_reg(const char* reg) {
    ILExpr* expr = this->expr(RDIL_REG);
    expr->reg = reg;
    return expr;
}

const ILExpr* ILExprPool::expr_sym(const char* sym) {
    ILExpr* expr = this->expr(RDIL_SYM);
    expr->sym = sym;
    return expr;
}

const ILExpr* ILExprPool::expr_cnst(u64 value) {
    ILExpr* expr = this->expr(RDIL_CNST);
    expr->u_value = value;
    return expr;
}

const ILExpr* ILExprPool::expr_var(RDAddress address) {
    ILExpr* expr = this->expr(RDIL_VAR);
    expr->address = address;
    return expr;
}

const ILExpr* ILExprPool::expr_goto(const ILExpr* e) {
    return this->expr_u(RDIL_GOTO, e);
}

const ILExpr* ILExprPool::expr_call(const ILExpr* e) {
    return this->expr_u(RDIL_CALL, e);
}

const ILExpr* ILExprPool::expr_ret(const ILExpr* e) {
    return this->expr_u(RDIL_RET, e);
}

const ILExpr* ILExprPool::expr_mem(const ILExpr* e) {
    return this->expr_u(RDIL_MEM, e);
}

const ILExpr* ILExprPool::expr_not(const ILExpr* e) {
    return this->expr_u(RDIL_NOT, e);
}

const ILExpr* ILExprPool::expr_add(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_ADD, l, r);
}

const ILExpr* ILExprPool::expr_sub(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_SUB, l, r);
}

const ILExpr* ILExprPool::expr_mul(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_MUL, l, r);
}

const ILExpr* ILExprPool::expr_div(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_DIV, l, r);
}

const ILExpr* ILExprPool::expr_mod(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_MOD, l, r);
}

const ILExpr* ILExprPool::expr_and(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_AND, l, r);
}

const ILExpr* ILExprPool::expr_or(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_OR, l, r);
}

const ILExpr* ILExprPool::expr_xor(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_XOR, l, r);
}

const ILExpr* ILExprPool::expr_lsl(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_LSL, l, r);
}

const ILExpr* ILExprPool::expr_lsr(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_LSR, l, r);
}

const ILExpr* ILExprPool::expr_asl(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_ASL, l, r);
}

const ILExpr* ILExprPool::expr_asr(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_ASR, l, r);
}

const ILExpr* ILExprPool::expr_rol(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_ROL, l, r);
}

const ILExpr* ILExprPool::expr_ror(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_ROR, l, r);
}

const ILExpr* ILExprPool::expr_copy(const ILExpr* dst, const ILExpr* src) {
    return this->expr_ds(RDIL_COPY, dst, src);
}

const ILExpr* ILExprPool::expr_if(const ILExpr* cond, const ILExpr* t,
                                  const ILExpr* f) {
    ILExpr* expr = this->expr(RDIL_IF);
    expr->cond = this->check(cond);
    expr->t = this->check(t);
    expr->f = this->check(f);
    return expr;
}

const ILExpr* ILExprPool::expr_eq(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_EQ, l, r);
}

const ILExpr* ILExprPool::expr_ne(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_NE, l, r);
}

const ILExpr* ILExprPool::expr_lt(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_LT, l, r);
}

const ILExpr* ILExprPool::expr_le(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_LE, l, r);
}

const ILExpr* ILExprPool::expr_gt(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_GT, l, r);
}

const ILExpr* ILExprPool::expr_ge(const ILExpr* l, const ILExpr* r) {
    return this->expr_lr(RDIL_GE, l, r);
}

const ILExpr* ILExprPool::expr_int(const ILExpr* e) {
    return this->expr_u(RDIL_INT, e);
}

const ILExpr* ILExprPool::check(const ILExpr* e) {
    return e ? e : this->expr_unknown();
}

ILExpr* ILExprPool::expr_ds(RDILOp op, const ILExpr* dst, const ILExpr* src) {
    ILExpr* expr = this->expr(op);
    expr->dst = this->check(dst);
    expr->src = this->check(src);
    return expr;
}

ILExpr* ILExprPool::expr_lr(RDILOp op, const ILExpr* l, const ILExpr* r) {
    ILExpr* expr = this->expr(op);
    expr->l = this->check(l);
    expr->r = this->check(r);
    return expr;
}

ILExpr* ILExprPool::expr_u(RDILOp op, const ILExpr* u) {
    ILExpr* expr = this->expr(op);
    expr->u = this->check(u);
    return expr;
}

ILExpr* ILExprPool::expr(RDILOp op) {
    auto& expr = m_pool.emplace_front();
    expr.op = op;
    return &expr;
}

} // namespace redasm::rdil
