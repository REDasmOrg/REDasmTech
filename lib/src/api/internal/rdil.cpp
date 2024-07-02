#include "rdil.h"
#include "../../rdil/expression.h"
#include "../marshal.h"

namespace redasm::api::internal {

const RDILExpression* rdiltree_unknown(RDILExpressionTree* self) {
    return api::to_c(api::from_c(self)->expr_unknown());
}

const RDILExpression* rdiltree_nop(RDILExpressionTree* self) {
    return api::to_c(api::from_c(self)->expr_nop());
}

const RDILExpression* rdiltree_var(RDILExpressionTree* self,
                                   RDAddress address) {
    return api::to_c(api::from_c(self)->expr_var(address));
}

const RDILExpression* rdiltree_reg(RDILExpressionTree* self, const char* reg) {
    return api::to_c(api::from_c(self)->expr_reg(reg));
}

const RDILExpression* rdiltree_cnst(RDILExpressionTree* self, u64 value) {
    return api::to_c(api::from_c(self)->expr_cnst(value));
}

const RDILExpression* rdiltree_add(RDILExpressionTree* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_add(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_sub(RDILExpressionTree* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_sub(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_mul(RDILExpressionTree* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_mul(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_div(RDILExpressionTree* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_div(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_mod(RDILExpressionTree* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_mod(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_and(RDILExpressionTree* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_and(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_or(RDILExpressionTree* self,
                                  const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_or(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_xor(RDILExpressionTree* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_xor(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_lsl(RDILExpressionTree* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_lsl(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_lsr(RDILExpressionTree* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_lsr(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_asl(RDILExpressionTree* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_asl(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_asr(RDILExpressionTree* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_asr(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_rol(RDILExpressionTree* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_rol(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_ror(RDILExpressionTree* self,
                                   const RDILExpression* l,
                                   const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_ror(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_not(RDILExpressionTree* self,
                                   const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_not(api::from_c(e)));
}

const RDILExpression* rdiltree_mem(RDILExpressionTree* self,
                                   const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_mem(api::from_c(e)));
}

const RDILExpression* rdiltree_copy(RDILExpressionTree* self,
                                    const RDILExpression* dst,
                                    const RDILExpression* src) {
    return api::to_c(
        api::from_c(self)->expr_copy(api::from_c(dst), api::from_c(src)));
}

const RDILExpression* rdiltree_goto(RDILExpressionTree* self,
                                    const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_goto(api::from_c(e)));
}

const RDILExpression* rdiltree_call(RDILExpressionTree* self,
                                    const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_call(api::from_c(e)));
}

const RDILExpression* rdiltree_ret(RDILExpressionTree* self,
                                   const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_ret(api::from_c(e)));
}

const RDILExpression* rdiltree_if(RDILExpressionTree* self,
                                  const RDILExpression* cond,
                                  const RDILExpression* t,
                                  const RDILExpression* f) {
    return api::to_c(api::from_c(self)->expr_if(
        api::from_c(cond), api::from_c(t), api::from_c(f)));
}

const RDILExpression* rdiltree_eq(RDILExpressionTree* self,
                                  const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_eq(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_ne(RDILExpressionTree* self,
                                  const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_ne(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_lt(RDILExpressionTree* self,
                                  const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_lt(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_le(RDILExpressionTree* self,
                                  const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_le(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_gt(RDILExpressionTree* self,
                                  const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_gt(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_ge(RDILExpressionTree* self,
                                  const RDILExpression* l,
                                  const RDILExpression* r) {
    return api::to_c(
        api::from_c(self)->expr_ge(api::from_c(l), api::from_c(r)));
}

const RDILExpression* rdiltree_push(RDILExpressionTree* self,
                                    const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_push(api::from_c(e)));
}

const RDILExpression* rdiltree_pop(RDILExpressionTree* self,
                                   const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_pop(api::from_c(e)));
}

const RDILExpression* rdiltree_int(RDILExpressionTree* self,
                                   const RDILExpression* e) {
    return api::to_c(api::from_c(self)->expr_int(api::from_c(e)));
}

} // namespace redasm::api::internal
