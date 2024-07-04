#include "rdil.h"
#include "../../rdil/expression.h"
#include "../../rdil/expressionlist.h"
#include "../marshal.h"

namespace redasm::api::internal {

const RDILExpression* rdilpool_unknown(RDILPool* self) {
    return api::to_c(api::from_c(self)->expr_unknown());
}

const RDILExpression* rdilpool_nop(RDILPool* self) {
    return api::to_c(api::from_c(self)->expr_nop());
}

const RDILExpression* rdilpool_var(RDILPool* self, RDAddress address) {
    return api::to_c(api::from_c(self)->expr_var(address));
}

const RDILExpression* rdilpool_reg(RDILPool* self, const char* reg) {
    return api::to_c(api::from_c(self)->expr_reg(reg));
}

const RDILExpression* rdilpool_cnst(RDILPool* self, u64 value) {
    return api::to_c(api::from_c(self)->expr_cnst(value));
}

const RDILExpression* rdilpool_add(RDILPool* self, const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_add(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_sub(RDILPool* self, const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_sub(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_mul(RDILPool* self, const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_mul(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_div(RDILPool* self, const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_div(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_mod(RDILPool* self, const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_mod(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_and(RDILPool* self, const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_and(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_or(RDILPool* self, const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_or(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_xor(RDILPool* self, const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_xor(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_lsl(RDILPool* self, const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_lsl(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_lsr(RDILPool* self, const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_lsr(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_asl(RDILPool* self, const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_asl(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_asr(RDILPool* self, const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_asr(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_rol(RDILPool* self, const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_rol(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_ror(RDILPool* self, const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_ror(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_not(RDILPool* self, const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_not(api::from_c(e)));
}

const RDILExpression* rdilpool_mem(RDILPool* self, const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_mem(api::from_c(e)));
}

const RDILExpression* rdilpool_copy(RDILPool* self, const RDILExpression* dst,
                                    const RDILExpression* src) {
    return api::to_c(
        api::from_c(self)->expr_copy(api::from_c(dst), api::from_c(src)));
}

const RDILExpression* rdilpool_goto(RDILPool* self, const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_goto(api::from_c(e)));
}

const RDILExpression* rdilpool_call(RDILPool* self, const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_call(api::from_c(e)));
}

const RDILExpression* rdilpool_ret(RDILPool* self, const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_ret(api::from_c(e)));
}

const RDILExpression* rdilpool_if(RDILPool* self, const RDILExpression* cond,
                                  const RDILExpression* t,
                                  const RDILExpression* f) {
    return api::to_c(api::from_c(self)->expr_if(
        api::from_c(cond), api::from_c(t), api::from_c(f)));
}

const RDILExpression* rdilpool_eq(RDILPool* self, const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_eq(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_ne(RDILPool* self, const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_ne(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_lt(RDILPool* self, const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_lt(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_le(RDILPool* self, const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_le(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_gt(RDILPool* self, const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_gt(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_ge(RDILPool* self, const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_ge(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_push(RDILPool* self, const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_push(api::from_c(e)));
}

const RDILExpression* rdilpool_pop(RDILPool* self, const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_pop(api::from_c(e)));
}

const RDILExpression* rdilpool_int(RDILPool* self, const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_int(api::from_c(e)));
}

RDILPool* rdillist_getpool(RDILList* self) {
    rdil::ILExpressionList* l = api::from_c(self);
    return api::to_c(static_cast<rdil::ILExpressionPool*>(l));
}

const RDILExpression* rdillist_at(const RDILList* self, usize idx) {
    return api::to_c(api::from_c(self)->at(idx));
}

void rdillist_append(RDILList* self, const RDILExpression* e) {
    api::from_c(self)->append(api::from_c(e));
}

bool rdillist_isempty(const RDILList* self) {
    return api::from_c(self)->empty();
}

usize rdillist_getsize(const RDILList* self) {
    return api::from_c(self)->size();
}

} // namespace redasm::api::internal
