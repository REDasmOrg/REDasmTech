#include "rdil.h"
#include "../../rdil/expression.h"
#include "../../rdil/expressionlist.h"
#include "../marshal.h"

namespace redasm::api::internal {

const RDILExpr* rdilpool_unknown(RDILPool* self) {
    return api::to_c(api::from_c(self)->expr_unknown());
}

const RDILExpr* rdilpool_nop(RDILPool* self) {
    return api::to_c(api::from_c(self)->expr_nop());
}

const RDILExpr* rdilpool_var(RDILPool* self, RDAddress address) {
    return api::to_c(api::from_c(self)->expr_var(address));
}

const RDILExpr* rdilpool_reg(RDILPool* self, int reg) {
    return api::to_c(api::from_c(self)->expr_reg(reg));
}

const RDILExpr* rdilpool_sym(RDILPool* self, const char* sym) {
    return api::to_c(api::from_c(self)->expr_sym(sym));
}

const RDILExpr* rdilpool_cnst(RDILPool* self, u64 value) {
    return api::to_c(api::from_c(self)->expr_cnst(value));
}

const RDILExpr* rdilpool_add(RDILPool* self, const RDILExpr* l,
                             const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_add(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_sub(RDILPool* self, const RDILExpr* l,
                             const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_sub(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_mul(RDILPool* self, const RDILExpr* l,
                             const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_mul(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_div(RDILPool* self, const RDILExpr* l,
                             const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_div(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_mod(RDILPool* self, const RDILExpr* l,
                             const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_mod(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_and(RDILPool* self, const RDILExpr* l,
                             const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_and(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_or(RDILPool* self, const RDILExpr* l,
                            const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_or(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_xor(RDILPool* self, const RDILExpr* l,
                             const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_xor(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_lsl(RDILPool* self, const RDILExpr* l,
                             const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_lsl(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_lsr(RDILPool* self, const RDILExpr* l,
                             const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_lsr(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_asl(RDILPool* self, const RDILExpr* l,
                             const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_asl(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_asr(RDILPool* self, const RDILExpr* l,
                             const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_asr(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_rol(RDILPool* self, const RDILExpr* l,
                             const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_rol(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_ror(RDILPool* self, const RDILExpr* l,
                             const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_ror(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_not(RDILPool* self, const RDILExpr* e) {
    return api::to_c(api::from_c(self)->expr_not(api::from_c(e)));
}

const RDILExpr* rdilpool_mem(RDILPool* self, const RDILExpr* e) {
    return api::to_c(api::from_c(self)->expr_mem(api::from_c(e)));
}

const RDILExpr* rdilpool_copy(RDILPool* self, const RDILExpr* dst,
                              const RDILExpr* src) {
    return api::to_c(
        api::from_c(self)->expr_copy(api::from_c(dst), api::from_c(src)));
}

const RDILExpr* rdilpool_goto(RDILPool* self, const RDILExpr* e) {
    return api::to_c(api::from_c(self)->expr_goto(api::from_c(e)));
}

const RDILExpr* rdilpool_call(RDILPool* self, const RDILExpr* e) {
    return api::to_c(api::from_c(self)->expr_call(api::from_c(e)));
}

const RDILExpr* rdilpool_ret(RDILPool* self, const RDILExpr* e) {
    return api::to_c(api::from_c(self)->expr_ret(api::from_c(e)));
}

const RDILExpr* rdilpool_if(RDILPool* self, const RDILExpr* cond,
                            const RDILExpr* t, const RDILExpr* f) {
    return api::to_c(api::from_c(self)->expr_if(
        api::from_c(cond), api::from_c(t), api::from_c(f)));
}

const RDILExpr* rdilpool_eq(RDILPool* self, const RDILExpr* l,
                            const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_eq(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_ne(RDILPool* self, const RDILExpr* l,
                            const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_ne(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_lt(RDILPool* self, const RDILExpr* l,
                            const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_lt(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_le(RDILPool* self, const RDILExpr* l,
                            const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_le(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_gt(RDILPool* self, const RDILExpr* l,
                            const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_gt(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_ge(RDILPool* self, const RDILExpr* l,
                            const RDILExpr* r) {
    return api::to_c(
        api::from_c(self)->expr_ge(api::from_c(l), api::from_c(r)));
}

const RDILExpr* rdilpool_push(RDILPool* self, const RDILExpr* e) {
    return api::to_c(api::from_c(self)->expr_push(api::from_c(e)));
}

const RDILExpr* rdilpool_pop(RDILPool* self, const RDILExpr* e) {
    return api::to_c(api::from_c(self)->expr_pop(api::from_c(e)));
}

const RDILExpr* rdilpool_int(RDILPool* self, const RDILExpr* e) {
    return api::to_c(api::from_c(self)->expr_int(api::from_c(e)));
}

RDILPool* rdillist_getpool(RDILList* self) {
    rdil::ILExprList* l = api::from_c(self);
    return api::to_c(static_cast<rdil::ILExprPool*>(l));
}

const RDILExpr* rdillist_at(const RDILList* self, usize idx) {
    return api::to_c(api::from_c(self)->at(idx));
}

void rdillist_append(RDILList* self, const RDILExpr* e) {
    api::from_c(self)->append(api::from_c(e));
}

bool rdillist_isempty(const RDILList* self) {
    return api::from_c(self)->empty();
}

usize rdillist_getsize(const RDILList* self) {
    return api::from_c(self)->size();
}

} // namespace redasm::api::internal
