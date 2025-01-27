#include "../../rdil/rdil.h"
#include "../../rdil/expression.h"
#include "../../rdil/expressionlist.h"
#include "../marshal.h"
#include <redasm/rdil.h>
#include <spdlog/spdlog.h>

const RDILExpr* rdil_unknown(RDILPool* self) {
    spdlog::trace("rdil_unknown({})", fmt::ptr(self));
    return redasm::api::from_c(self)->expr_unknown();
}

const RDILExpr* rdil_nop(RDILPool* self) {
    spdlog::trace("rdil_nop({})", fmt::ptr(self));
    return redasm::api::from_c(self)->expr_nop();
}

const RDILExpr* rdil_var(RDILPool* self, RDAddress address) {
    spdlog::trace("rdil_var({}, {:x})", fmt::ptr(self), address);
    return redasm::api::from_c(self)->expr_var(address);
}

const RDILExpr* rdil_reg(RDILPool* self, int reg) {
    spdlog::trace("rdil_reg({}, {})", fmt::ptr(self), reg);
    return redasm::api::from_c(self)->expr_reg(reg);
}

const RDILExpr* rdil_sym(RDILPool* self, const char* sym) {
    spdlog::trace("rdil_sym({}, '{}')", fmt::ptr(self), sym);
    return redasm::api::from_c(self)->expr_sym(sym);
}

const RDILExpr* rdil_cnst(RDILPool* self, u64 value) {
    spdlog::trace("rdil_cnst({}, {})", fmt::ptr(self), value);
    return redasm::api::from_c(self)->expr_cnst(value);
}

const RDILExpr* rdil_add(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_add({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_add(l, r);
}

const RDILExpr* rdil_sub(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_sub({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_sub(l, r);
}

const RDILExpr* rdil_mul(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_mul({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_mul(l, r);
}

const RDILExpr* rdil_div(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_div({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_div(l, r);
}

const RDILExpr* rdil_mod(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_mod({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_mod(l, r);
}

const RDILExpr* rdil_and(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_and({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_and(l, r);
}

const RDILExpr* rdil_or(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_or({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_or(l, r);
}

const RDILExpr* rdil_xor(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_xor({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_xor(l, r);
}

const RDILExpr* rdil_lsl(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_lsl({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_lsl(l, r);
}

const RDILExpr* rdil_lsr(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_lsr({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_lsr(l, r);
}

const RDILExpr* rdil_asl(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_asl({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_asl(l, r);
}

const RDILExpr* rdil_asr(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_asr({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_asr(l, r);
}

const RDILExpr* rdil_rol(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_rol({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_rol(l, r);
}

const RDILExpr* rdil_ror(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_ror({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_ror(l, r);
}

const RDILExpr* rdil_not(RDILPool* self, const RDILExpr* e) {
    spdlog::trace("rdil_not({}, {})", fmt::ptr(self), fmt::ptr(e));
    return redasm::api::from_c(self)->expr_not(e);
}

const RDILExpr* rdil_mem(RDILPool* self, const RDILExpr* e) {
    spdlog::trace("rdil_mem({}, {})", fmt::ptr(self), fmt::ptr(e));
    return redasm::api::from_c(self)->expr_mem(e);
}

const RDILExpr* rdil_copy(RDILPool* self, const RDILExpr* dst,
                          const RDILExpr* src) {
    spdlog::trace("rdil_copy({}, {})", fmt::ptr(self), fmt::ptr(dst),
                  fmt::ptr(src));
    return redasm::api::from_c(self)->expr_copy(dst, src);
}

const RDILExpr* rdil_goto(RDILPool* self, const RDILExpr* e) {
    spdlog::trace("rdil_goto({}, {})", fmt::ptr(self), fmt::ptr(e));
    return redasm::api::from_c(self)->expr_goto(e);
}

const RDILExpr* rdil_call(RDILPool* self, const RDILExpr* e) {
    spdlog::trace("rdil_call({}, {})", fmt::ptr(self), fmt::ptr(e));
    return redasm::api::from_c(self)->expr_call(e);
}

const RDILExpr* rdil_ret(RDILPool* self, const RDILExpr* e) {
    spdlog::trace("rdil_ret({}, {})", fmt::ptr(self), fmt::ptr(e));
    return redasm::api::from_c(self)->expr_ret(e);
}

const RDILExpr* rdil_if(RDILPool* self, const RDILExpr* cond, const RDILExpr* t,
                        const RDILExpr* f) {
    spdlog::trace("rdil_if({}, {})", fmt::ptr(self), fmt::ptr(t), fmt::ptr(f));
    return redasm::api::from_c(self)->expr_if(cond, t, f);
}

const RDILExpr* rdil_eq(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_eq({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_eq(l, r);
}

const RDILExpr* rdil_ne(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_ne({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_ne(l, r);
}

const RDILExpr* rdil_lt(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_lt({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_lt(l, r);
}

const RDILExpr* rdil_le(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_le({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_le(l, r);
}

const RDILExpr* rdil_gt(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_gt({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_gt(l, r);
}

const RDILExpr* rdil_ge(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    spdlog::trace("rdil_ge({}, {})", fmt::ptr(self), fmt::ptr(l), fmt::ptr(r));
    return redasm::api::from_c(self)->expr_ge(l, r);
}

const RDILExpr* rdil_push(RDILPool* self, const RDILExpr* e) {
    spdlog::trace("rdil_push({}, {})", fmt::ptr(self), fmt::ptr(e));
    return redasm::api::from_c(self)->expr_push(e);
}

const RDILExpr* rdil_pop(RDILPool* self, const RDILExpr* e) {
    spdlog::trace("rdil_pop({}, {})", fmt::ptr(self), fmt::ptr(e));
    return redasm::api::from_c(self)->expr_pop(e);
}

const RDILExpr* rdil_int(RDILPool* self, const RDILExpr* e) {
    spdlog::trace("rdil_int({}, {})", fmt::ptr(self), fmt::ptr(e));
    return redasm::api::from_c(self)->expr_int(e);
}

RDILList* rdilist_create() {
    spdlog::trace("rdil_create()");
    return redasm::api::to_c(new redasm::rdil::ILExprList());
}

RDILPool* rdillist_getpool(RDILList* self) {
    spdlog::trace("rdillist_getpool({})", fmt::ptr(self));
    redasm::rdil::ILExprList* l = redasm::api::from_c(self);
    return redasm::api::to_c(static_cast<redasm::rdil::ILExprPool*>(l));
}

void rdil_generate(const RDFunction* f, RDILList* l) {
    spdlog::trace("rdil_generate({}, {})", fmt::ptr(f), fmt::ptr(l));

    if(f) {
        redasm::rdil::generate(*redasm::api::from_c(f),
                               *redasm::api::from_c(l));
    }
}

const RDILExpr* rdillist_at(const RDILList* self, usize idx) {
    spdlog::trace("rdillist_at({}, {})", fmt::ptr(self), idx);
    return redasm::api::from_c(self)->at(idx);
}

void rdillist_append(RDILList* self, const RDILExpr* e) {
    spdlog::trace("rdillist_append({}, {})", fmt::ptr(self), fmt::ptr(e));
    redasm::api::from_c(self)->append(e);
}

bool rdillist_isempty(const RDILList* self) {
    spdlog::trace("rdillist_isempty({})", fmt::ptr(self));
    return redasm::api::from_c(self)->empty();
}

usize rdillist_getsize(const RDILList* self) {
    spdlog::trace("rdillist_getsize({})", fmt::ptr(self));
    return redasm::api::from_c(self)->size();
}
