#include "../internal/rdil.h"
#include <redasm/rdil.h>

const RDILExpression* rdil_unknown(RDILExpressionTree* self) {
    return redasm::api::internal::rdiltree_unknown(self);
}

const RDILExpression* rdil_nop(RDILExpressionTree* self) {
    return redasm::api::internal::rdiltree_nop(self);
}

const RDILExpression* rdil_var(RDILExpressionTree* self, RDAddress address) {
    return redasm::api::internal::rdiltree_var(self, address);
}

const RDILExpression* rdil_reg(RDILExpressionTree* self, const char* reg) {
    return redasm::api::internal::rdiltree_reg(self, reg);
}

const RDILExpression* rdil_cnst(RDILExpressionTree* self, u64 value) {
    return redasm::api::internal::rdiltree_cnst(self, value);
}

const RDILExpression* rdil_add(RDILExpressionTree* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdiltree_add(self, l, r);
}

const RDILExpression* rdil_sub(RDILExpressionTree* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdiltree_sub(self, l, r);
}

const RDILExpression* rdil_mul(RDILExpressionTree* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdiltree_mul(self, l, r);
}

const RDILExpression* rdil_div(RDILExpressionTree* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdiltree_div(self, l, r);
}

const RDILExpression* rdil_mod(RDILExpressionTree* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdiltree_mod(self, l, r);
}

const RDILExpression* rdil_and(RDILExpressionTree* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdiltree_and(self, l, r);
}

const RDILExpression* rdil_or(RDILExpressionTree* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdiltree_or(self, l, r);
}

const RDILExpression* rdil_xor(RDILExpressionTree* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdiltree_xor(self, l, r);
}

const RDILExpression* rdil_lsl(RDILExpressionTree* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdiltree_lsl(self, l, r);
}

const RDILExpression* rdil_lsr(RDILExpressionTree* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdiltree_lsr(self, l, r);
}

const RDILExpression* rdil_asl(RDILExpressionTree* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdiltree_asl(self, l, r);
}

const RDILExpression* rdil_asr(RDILExpressionTree* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdiltree_asr(self, l, r);
}

const RDILExpression* rdil_rol(RDILExpressionTree* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdiltree_rol(self, l, r);
}

const RDILExpression* rdil_ror(RDILExpressionTree* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdiltree_ror(self, l, r);
}

const RDILExpression* rdil_not(RDILExpressionTree* self,
                               const RDILExpression* e) {
    return redasm::api::internal::rdiltree_not(self, e);
}

const RDILExpression* rdil_mem(RDILExpressionTree* self,
                               const RDILExpression* e) {
    return redasm::api::internal::rdiltree_mem(self, e);
}

const RDILExpression* rdil_copy(RDILExpressionTree* self,
                                const RDILExpression* dst,
                                const RDILExpression* src) {
    return redasm::api::internal::rdiltree_copy(self, dst, src);
}

const RDILExpression* rdil_goto(RDILExpressionTree* self,
                                const RDILExpression* e) {
    return redasm::api::internal::rdiltree_goto(self, e);
}

const RDILExpression* rdil_call(RDILExpressionTree* self,
                                const RDILExpression* e) {
    return redasm::api::internal::rdiltree_call(self, e);
}

const RDILExpression* rdil_ret(RDILExpressionTree* self,
                               const RDILExpression* e) {
    return redasm::api::internal::rdiltree_ret(self, e);
}

const RDILExpression* rdil_if(RDILExpressionTree* self,
                              const RDILExpression* cond,
                              const RDILExpression* t,
                              const RDILExpression* f) {
    return redasm::api::internal::rdiltree_if(self, cond, t, f);
}

const RDILExpression* rdil_eq(RDILExpressionTree* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdiltree_eq(self, l, r);
}

const RDILExpression* rdil_ne(RDILExpressionTree* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdiltree_ne(self, l, r);
}

const RDILExpression* rdil_lt(RDILExpressionTree* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdiltree_lt(self, l, r);
}

const RDILExpression* rdil_le(RDILExpressionTree* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdiltree_le(self, l, r);
}

const RDILExpression* rdil_gt(RDILExpressionTree* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdiltree_gt(self, l, r);
}

const RDILExpression* rdil_ge(RDILExpressionTree* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdiltree_ge(self, l, r);
}

const RDILExpression* rdil_push(RDILExpressionTree* self,
                                const RDILExpression* e) {
    return redasm::api::internal::rdiltree_push(self, e);
}

const RDILExpression* rdil_pop(RDILExpressionTree* self,
                               const RDILExpression* e) {
    return redasm::api::internal::rdiltree_pop(self, e);
}

const RDILExpression* rdil_int(RDILExpressionTree* self,
                               const RDILExpression* e) {
    return redasm::api::internal::rdiltree_int(self, e);
}
