#include "expression.h"

namespace redasm::rdil {

const RDILExpr* ILExprPool::expr_unknown() {
    static RDILExpr expr{};
    expr.op = RDIL_UNKNOWN;
    return &expr;
}

const RDILExpr* ILExprPool::expr_nop() {
    static RDILExpr expr{};
    expr.op = RDIL_NOP;
    return &expr;
}

const RDILExpr* ILExprPool::expr_pop(const RDILExpr* e) {
    return this->expr_u(RDIL_POP, e);
}

const RDILExpr* ILExprPool::expr_push(const RDILExpr* e) {
    return this->expr_u(RDIL_PUSH, e);
}

const RDILExpr* ILExprPool::expr_reg(int reg) {
    RDILExpr* expr = this->expr(RDIL_REG);
    expr->reg = reg;
    return expr;
}

const RDILExpr* ILExprPool::expr_sym(const char* sym) {
    RDILExpr* expr = this->expr(RDIL_SYM);
    expr->sym = sym;
    return expr;
}

const RDILExpr* ILExprPool::expr_cnst(u64 value) {
    RDILExpr* expr = this->expr(RDIL_CNST);
    expr->u_value = value;
    return expr;
}

const RDILExpr* ILExprPool::expr_var(RDAddress address) {
    RDILExpr* expr = this->expr(RDIL_VAR);
    expr->address = address;
    return expr;
}

const RDILExpr* ILExprPool::expr_goto(const RDILExpr* e) {
    return this->expr_u(RDIL_GOTO, e);
}

const RDILExpr* ILExprPool::expr_call(const RDILExpr* e) {
    return this->expr_u(RDIL_CALL, e);
}

const RDILExpr* ILExprPool::expr_ret(const RDILExpr* e) {
    return this->expr_u(RDIL_RET, e);
}

const RDILExpr* ILExprPool::expr_mem(const RDILExpr* e) {
    return this->expr_u(RDIL_MEM, e);
}

const RDILExpr* ILExprPool::expr_not(const RDILExpr* e) {
    return this->expr_u(RDIL_NOT, e);
}

const RDILExpr* ILExprPool::expr_add(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_ADD, l, r);
}

const RDILExpr* ILExprPool::expr_sub(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_SUB, l, r);
}

const RDILExpr* ILExprPool::expr_mul(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_MUL, l, r);
}

const RDILExpr* ILExprPool::expr_div(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_DIV, l, r);
}

const RDILExpr* ILExprPool::expr_mod(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_MOD, l, r);
}

const RDILExpr* ILExprPool::expr_and(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_AND, l, r);
}

const RDILExpr* ILExprPool::expr_or(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_OR, l, r);
}

const RDILExpr* ILExprPool::expr_xor(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_XOR, l, r);
}

const RDILExpr* ILExprPool::expr_lsl(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_LSL, l, r);
}

const RDILExpr* ILExprPool::expr_lsr(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_LSR, l, r);
}

const RDILExpr* ILExprPool::expr_asl(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_ASL, l, r);
}

const RDILExpr* ILExprPool::expr_asr(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_ASR, l, r);
}

const RDILExpr* ILExprPool::expr_rol(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_ROL, l, r);
}

const RDILExpr* ILExprPool::expr_ror(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_ROR, l, r);
}

const RDILExpr* ILExprPool::expr_copy(const RDILExpr* dst,
                                      const RDILExpr* src) {
    return this->expr_ds(RDIL_COPY, dst, src);
}

const RDILExpr* ILExprPool::expr_if(const RDILExpr* cond, const RDILExpr* t,
                                    const RDILExpr* f) {
    RDILExpr* expr = this->expr(RDIL_IF);
    expr->cond = this->check(cond);
    expr->t = this->check(t);
    expr->f = this->check(f);
    return expr;
}

const RDILExpr* ILExprPool::expr_eq(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_EQ, l, r);
}

const RDILExpr* ILExprPool::expr_ne(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_NE, l, r);
}

const RDILExpr* ILExprPool::expr_lt(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_LT, l, r);
}

const RDILExpr* ILExprPool::expr_le(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_LE, l, r);
}

const RDILExpr* ILExprPool::expr_gt(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_GT, l, r);
}

const RDILExpr* ILExprPool::expr_ge(const RDILExpr* l, const RDILExpr* r) {
    return this->expr_lr(RDIL_GE, l, r);
}

const RDILExpr* ILExprPool::expr_int(const RDILExpr* e) {
    return this->expr_u(RDIL_INT, e);
}

const RDILExpr* ILExprPool::check(const RDILExpr* e) {
    return e ? e : this->expr_unknown();
}

RDILExpr* ILExprPool::expr_ds(RDILOp op, const RDILExpr* dst,
                              const RDILExpr* src) {
    RDILExpr* expr = this->expr(op);
    expr->dst = this->check(dst);
    expr->src = this->check(src);
    return expr;
}

RDILExpr* ILExprPool::expr_lr(RDILOp op, const RDILExpr* l, const RDILExpr* r) {
    RDILExpr* expr = this->expr(op);
    expr->l = this->check(l);
    expr->r = this->check(r);
    return expr;
}

RDILExpr* ILExprPool::expr_u(RDILOp op, const RDILExpr* u) {
    RDILExpr* expr = this->expr(op);
    expr->u = this->check(u);
    return expr;
}

RDILExpr* ILExprPool::expr(RDILOp op) {
    auto& expr = m_pool.emplace_front();
    expr.op = op;
    return &expr;
}

} // namespace redasm::rdil
