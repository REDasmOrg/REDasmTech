#include "../internal/rdil.h"
#include <redasm/rdil.h>

const RDILExpression* rdil_unknown(RDILPool* self) {
    return redasm::api::internal::rdilpool_unknown(self);
}

const RDILExpression* rdil_nop(RDILPool* self) {
    return redasm::api::internal::rdilpool_nop(self);
}

const RDILExpression* rdil_var(RDILPool* self, RDAddress address) {
    return redasm::api::internal::rdilpool_var(self, address);
}

const RDILExpression* rdil_reg(RDILPool* self, const char* reg) {
    return redasm::api::internal::rdilpool_reg(self, reg);
}

const RDILExpression* rdil_cnst(RDILPool* self, u64 value) {
    return redasm::api::internal::rdilpool_cnst(self, value);
}

const RDILExpression* rdil_add(RDILPool* self, const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_add(self, l, r);
}

const RDILExpression* rdil_sub(RDILPool* self, const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_sub(self, l, r);
}

const RDILExpression* rdil_mul(RDILPool* self, const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_mul(self, l, r);
}

const RDILExpression* rdil_div(RDILPool* self, const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_div(self, l, r);
}

const RDILExpression* rdil_mod(RDILPool* self, const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_mod(self, l, r);
}

const RDILExpression* rdil_and(RDILPool* self, const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_and(self, l, r);
}

const RDILExpression* rdil_or(RDILPool* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdilpool_or(self, l, r);
}

const RDILExpression* rdil_xor(RDILPool* self, const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_xor(self, l, r);
}

const RDILExpression* rdil_lsl(RDILPool* self, const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_lsl(self, l, r);
}

const RDILExpression* rdil_lsr(RDILPool* self, const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_lsr(self, l, r);
}

const RDILExpression* rdil_asl(RDILPool* self, const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_asl(self, l, r);
}

const RDILExpression* rdil_asr(RDILPool* self, const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_asr(self, l, r);
}

const RDILExpression* rdil_rol(RDILPool* self, const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_rol(self, l, r);
}

const RDILExpression* rdil_ror(RDILPool* self, const RDILExpression* l,
                               const RDILExpression* r) {
    return redasm::api::internal::rdilpool_ror(self, l, r);
}

const RDILExpression* rdil_not(RDILPool* self, const RDILExpression* e) {
    return redasm::api::internal::rdilpool_not(self, e);
}

const RDILExpression* rdil_mem(RDILPool* self, const RDILExpression* e) {
    return redasm::api::internal::rdilpool_mem(self, e);
}

const RDILExpression* rdil_copy(RDILPool* self, const RDILExpression* dst,
                                const RDILExpression* src) {
    return redasm::api::internal::rdilpool_copy(self, dst, src);
}

const RDILExpression* rdil_goto(RDILPool* self, const RDILExpression* e) {
    return redasm::api::internal::rdilpool_goto(self, e);
}

const RDILExpression* rdil_call(RDILPool* self, const RDILExpression* e) {
    return redasm::api::internal::rdilpool_call(self, e);
}

const RDILExpression* rdil_ret(RDILPool* self, const RDILExpression* e) {
    return redasm::api::internal::rdilpool_ret(self, e);
}

const RDILExpression* rdil_if(RDILPool* self, const RDILExpression* cond,
                              const RDILExpression* t,
                              const RDILExpression* f) {
    return redasm::api::internal::rdilpool_if(self, cond, t, f);
}

const RDILExpression* rdil_eq(RDILPool* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdilpool_eq(self, l, r);
}

const RDILExpression* rdil_ne(RDILPool* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdilpool_ne(self, l, r);
}

const RDILExpression* rdil_lt(RDILPool* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdilpool_lt(self, l, r);
}

const RDILExpression* rdil_le(RDILPool* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdilpool_le(self, l, r);
}

const RDILExpression* rdil_gt(RDILPool* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdilpool_gt(self, l, r);
}

const RDILExpression* rdil_ge(RDILPool* self, const RDILExpression* l,
                              const RDILExpression* r) {
    return redasm::api::internal::rdilpool_ge(self, l, r);
}

const RDILExpression* rdil_push(RDILPool* self, const RDILExpression* e) {
    return redasm::api::internal::rdilpool_push(self, e);
}

const RDILExpression* rdil_pop(RDILPool* self, const RDILExpression* e) {
    return redasm::api::internal::rdilpool_pop(self, e);
}

const RDILExpression* rdil_int(RDILPool* self, const RDILExpression* e) {
    return redasm::api::internal::rdilpool_int(self, e);
}

RDILPool* rdillist_getpool(RDILList* self) {
    return redasm::api::internal::rdillist_getpool(self);
}

const RDILExpression* rdillist_at(const RDILList* self, usize idx) {
    return redasm::api::internal::rdillist_at(self, idx);
}

void rdillist_append(RDILList* self, const RDILExpression* e) {
    redasm::api::internal::rdillist_append(self, e);
}

bool rdillist_isempty(const RDILList* self) {
    return redasm::api::internal::rdillist_isempty(self);
}

usize rdillist_getsize(const RDILList* self) {
    return redasm::api::internal::rdillist_getsize(self);
}
