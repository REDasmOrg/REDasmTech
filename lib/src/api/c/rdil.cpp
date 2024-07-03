#include "../internal/rdil.h"
#include <redasm/rdil.h>

const RDILExpression* rdil_unknown(RDILExpressionPool* self) {
    return redasm::api::internal::rdilpool_unknown(self);
}

const RDILExpression* rdil_nop(RDILExpressionPool* self) {
    return redasm::api::internal::rdilpool_nop(self);
}

const RDILExpression* rdil_var(RDILExpressionPool* self, RDAddress address) {
    return redasm::api::internal::rdilpool_var(self, address);
}

const RDILExpression* rdil_reg(RDILExpressionPool* self, const char* reg) {
    return redasm::api::internal::rdilpool_reg(self, reg);
}

const RDILExpression* rdil_cnst(RDILExpressionPool* self, u64 value) {
    return redasm::api::internal::rdilpool_cnst(self, value);
}

const RDILExpression* rdil_add(RDILExpressionPool* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_add(self, l, r);
}

const RDILExpression* rdil_sub(RDILExpressionPool* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_sub(self, l, r);
}

const RDILExpression* rdil_mul(RDILExpressionPool* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_mul(self, l, r);
}

const RDILExpression* rdil_div(RDILExpressionPool* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_div(self, l, r);
}

const RDILExpression* rdil_mod(RDILExpressionPool* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_mod(self, l, r);
}

const RDILExpression* rdil_and(RDILExpressionPool* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_and(self, l, r);
}

const RDILExpression* rdil_or(RDILExpressionPool* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdilpool_or(self, l, r);
}

const RDILExpression* rdil_xor(RDILExpressionPool* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_xor(self, l, r);
}

const RDILExpression* rdil_lsl(RDILExpressionPool* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_lsl(self, l, r);
}

const RDILExpression* rdil_lsr(RDILExpressionPool* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_lsr(self, l, r);
}

const RDILExpression* rdil_asl(RDILExpressionPool* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_asl(self, l, r);
}

const RDILExpression* rdil_asr(RDILExpressionPool* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_asr(self, l, r);
}

const RDILExpression* rdil_rol(RDILExpressionPool* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_rol(self, l, r);
}

const RDILExpression* rdil_ror(RDILExpressionPool* self,
                               const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_ror(self, l, r);
}

const RDILExpression* rdil_not(RDILExpressionPool* self,
                               const RDILExpression* e) {
    return redasm::api::internal::rdilpool_not(self, e);
}

const RDILExpression* rdil_mem(RDILExpressionPool* self,
                               const RDILExpression* e) {
    return redasm::api::internal::rdilpool_mem(self, e);
}

const RDILExpression* rdil_copy(RDILExpressionPool* self,
                                const RDILExpression* dst,
                                const RDILExpression* src) {
    return redasm::api::internal::rdilpool_copy(self, dst, src);
}

const RDILExpression* rdil_goto(RDILExpressionPool* self,
                                const RDILExpression* e) {
    return redasm::api::internal::rdilpool_goto(self, e);
}

const RDILExpression* rdil_call(RDILExpressionPool* self,
                                const RDILExpression* e) {
    return redasm::api::internal::rdilpool_call(self, e);
}

const RDILExpression* rdil_ret(RDILExpressionPool* self,
                               const RDILExpression* e) {
    return redasm::api::internal::rdilpool_ret(self, e);
}

const RDILExpression* rdil_if(RDILExpressionPool* self,
                              const RDILExpression* cond,
                              const RDILExpression* t,
                              const RDILExpression* f) {
    return redasm::api::internal::rdilpool_if(self, cond, t, f);
}

const RDILExpression* rdil_eq(RDILExpressionPool* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdilpool_eq(self, l, r);
}

const RDILExpression* rdil_ne(RDILExpressionPool* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdilpool_ne(self, l, r);
}

const RDILExpression* rdil_lt(RDILExpressionPool* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdilpool_lt(self, l, r);
}

const RDILExpression* rdil_le(RDILExpressionPool* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdilpool_le(self, l, r);
}

const RDILExpression* rdil_gt(RDILExpressionPool* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdilpool_gt(self, l, r);
}

const RDILExpression* rdil_ge(RDILExpressionPool* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdilpool_ge(self, l, r);
}

const RDILExpression* rdil_push(RDILExpressionPool* self,
                                const RDILExpression* e) {
    return redasm::api::internal::rdilpool_push(self, e);
}

const RDILExpression* rdil_pop(RDILExpressionPool* self,
                               const RDILExpression* e) {
    return redasm::api::internal::rdilpool_pop(self, e);
}

const RDILExpression* rdil_int(RDILExpressionPool* self,
                               const RDILExpression* e) {
    return redasm::api::internal::rdilpool_int(self, e);
}

RDILExpressionPool* rdilexpressionlist_getpool(RDILExpressionList* self) {
    return redasm::api::internal::rdillist_getpool(self);
}

const RDILExpression* rdilexpressionlist_at(const RDILExpressionList* self,
                                            usize idx) {
    return redasm::api::internal::rdillist_at(self, idx);
}

void rdilexpressionlist_append(RDILExpressionList* self,
                               const RDILExpression* e) {
    redasm::api::internal::rdillist_append(self, e);
}

bool rdilexpressionlist_isempty(const RDILExpressionList* self) {
    return redasm::api::internal::rdillist_isempty(self);
}

usize rdilexpressionlist_getsize(const RDILExpressionList* self) {
    return redasm::api::internal::rdillist_getsize(self);
}
