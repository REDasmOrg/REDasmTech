#include "rdil.h"
#include "../../rdil/expression.h"
#include "../../rdil/expressionlist.h"
#include "../marshal.h"

namespace redasm::api::internal {

const RDILExpression* rdilpool_unknown(RDILExpressionPool* self) {
    return api::to_c(api::from_c(self)->expr_unknown());
}

const RDILExpression* rdilpool_nop(RDILExpressionPool* self) {
    return api::to_c(api::from_c(self)->expr_nop());
}

const RDILExpression* rdilpool_var(RDILExpressionPool* self,
                                   RDAddress address) {
    return api::to_c(api::from_c(self)->expr_var(address));
}

const RDILExpression* rdilpool_reg(RDILExpressionPool* self, const char* reg) {
    return api::to_c(api::from_c(self)->expr_reg(reg));
}

const RDILExpression* rdilpool_cnst(RDILExpressionPool* self, u64 value) {
    return api::to_c(api::from_c(self)->expr_cnst(value));
}

const RDILExpression* rdilpool_add(RDILExpressionPool* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_add(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_sub(RDILExpressionPool* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_sub(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_mul(RDILExpressionPool* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_mul(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_div(RDILExpressionPool* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_div(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_mod(RDILExpressionPool* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_mod(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_and(RDILExpressionPool* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_and(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_or(RDILExpressionPool* self,
                                  const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_or(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_xor(RDILExpressionPool* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_xor(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_lsl(RDILExpressionPool* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_lsl(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_lsr(RDILExpressionPool* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_lsr(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_asl(RDILExpressionPool* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_asl(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_asr(RDILExpressionPool* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_asr(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_rol(RDILExpressionPool* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_rol(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_ror(RDILExpressionPool* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_ror(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_not(RDILExpressionPool* self,
                                   const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_not(api::from_c(e)));
}

const RDILExpression* rdilpool_mem(RDILExpressionPool* self,
                                   const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_mem(api::from_c(e)));
}

const RDILExpression* rdilpool_copy(RDILExpressionPool* self,
                                    const RDILExpression* dst,
                                    const RDILExpression* src) {
    return api::to_c(
        api::from_c(self)->expr_copy(api::from_c(dst), api::from_c(src)));
}

const RDILExpression* rdilpool_goto(RDILExpressionPool* self,
                                    const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_goto(api::from_c(e)));
}

const RDILExpression* rdilpool_call(RDILExpressionPool* self,
                                    const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_call(api::from_c(e)));
}

const RDILExpression* rdilpool_ret(RDILExpressionPool* self,
                                   const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_ret(api::from_c(e)));
}

const RDILExpression* rdilpool_if(RDILExpressionPool* self,
                                  const RDILExpression* cond,
                                  const RDILExpression* t,
                                  const RDILExpression* f) {
    return api::to_c(api::from_c(self)->expr_if(
        api::from_c(cond), api::from_c(t), api::from_c(f)));
}

const RDILExpression* rdilpool_eq(RDILExpressionPool* self,
                                  const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_eq(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_ne(RDILExpressionPool* self,
                                  const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_ne(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_lt(RDILExpressionPool* self,
                                  const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_lt(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_le(RDILExpressionPool* self,
                                  const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_le(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_gt(RDILExpressionPool* self,
                                  const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_gt(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_ge(RDILExpressionPool* self,
                                  const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_ge(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdilpool_push(RDILExpressionPool* self,
                                    const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_push(api::from_c(e)));
}

const RDILExpression* rdilpool_pop(RDILExpressionPool* self,
                                   const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_pop(api::from_c(e)));
}

const RDILExpression* rdilpool_int(RDILExpressionPool* self,
                                   const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_int(api::from_c(e)));
}

RDILExpressionPool* rdillist_getpool(RDILExpressionList* self) {
    rdil::ILExpressionList* l = api::from_c(self);
    return api::to_c(static_cast<rdil::ILExpressionPool*>(l));
}

const RDILExpression* rdillist_at(const RDILExpressionList* self, usize idx) {
    return api::to_c(api::from_c(self)->at(idx));
}

void rdillist_append(RDILExpressionList* self, const RDILExpression* e) {
    api::from_c(self)->append(api::from_c(e));
}

bool rdillist_isempty(const RDILExpressionList* self) {
    return api::from_c(self)->empty();
}

usize rdillist_getsize(const RDILExpressionList* self) {
    return api::from_c(self)->size();
}

} // namespace redasm::api::internal
