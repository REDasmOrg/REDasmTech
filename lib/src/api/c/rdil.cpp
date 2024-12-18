#include "../internal/rdil.h"
#include <redasm/rdil.h>

const RDILExpr* rdil_unknown(RDILPool* self) {
    return redasm::api::internal::rdilpool_unknown(self);
}

const RDILExpr* rdil_nop(RDILPool* self) {
    return redasm::api::internal::rdilpool_nop(self);
}

const RDILExpr* rdil_var(RDILPool* self, RDAddress address) {
    return redasm::api::internal::rdilpool_var(self, address);
}

const RDILExpr* rdil_reg(RDILPool* self, int reg) {
    return redasm::api::internal::rdilpool_reg(self, reg);
}

const RDILExpr* rdil_sym(RDILPool* self, const char* sym) {
    return redasm::api::internal::rdilpool_sym(self, sym);
}

const RDILExpr* rdil_cnst(RDILPool* self, u64 value) {
    return redasm::api::internal::rdilpool_cnst(self, value);
}

const RDILExpr* rdil_add(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_add(self, l, r);
}

const RDILExpr* rdil_sub(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_sub(self, l, r);
}

const RDILExpr* rdil_mul(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_mul(self, l, r);
}

const RDILExpr* rdil_div(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_div(self, l, r);
}

const RDILExpr* rdil_mod(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_mod(self, l, r);
}

const RDILExpr* rdil_and(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_and(self, l, r);
}

const RDILExpr* rdil_or(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_or(self, l, r);
}

const RDILExpr* rdil_xor(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_xor(self, l, r);
}

const RDILExpr* rdil_lsl(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_lsl(self, l, r);
}

const RDILExpr* rdil_lsr(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_lsr(self, l, r);
}

const RDILExpr* rdil_asl(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_asl(self, l, r);
}

const RDILExpr* rdil_asr(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_asr(self, l, r);
}

const RDILExpr* rdil_rol(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_rol(self, l, r);
}

const RDILExpr* rdil_ror(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_ror(self, l, r);
}

const RDILExpr* rdil_not(RDILPool* self, const RDILExpr* e) {
    return redasm::api::internal::rdilpool_not(self, e);
}

const RDILExpr* rdil_mem(RDILPool* self, const RDILExpr* e) {
    return redasm::api::internal::rdilpool_mem(self, e);
}

const RDILExpr* rdil_copy(RDILPool* self, const RDILExpr* dst,
                          const RDILExpr* src) {
    return redasm::api::internal::rdilpool_copy(self, dst, src);
}

const RDILExpr* rdil_goto(RDILPool* self, const RDILExpr* e) {
    return redasm::api::internal::rdilpool_goto(self, e);
}

const RDILExpr* rdil_call(RDILPool* self, const RDILExpr* e) {
    return redasm::api::internal::rdilpool_call(self, e);
}

const RDILExpr* rdil_ret(RDILPool* self, const RDILExpr* e) {
    return redasm::api::internal::rdilpool_ret(self, e);
}

const RDILExpr* rdil_if(RDILPool* self, const RDILExpr* cond, const RDILExpr* t,
                        const RDILExpr* f) {
    return redasm::api::internal::rdilpool_if(self, cond, t, f);
}

const RDILExpr* rdil_eq(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_eq(self, l, r);
}

const RDILExpr* rdil_ne(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_ne(self, l, r);
}

const RDILExpr* rdil_lt(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_lt(self, l, r);
}

const RDILExpr* rdil_le(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_le(self, l, r);
}

const RDILExpr* rdil_gt(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_gt(self, l, r);
}

const RDILExpr* rdil_ge(RDILPool* self, const RDILExpr* l, const RDILExpr* r) {
    return redasm::api::internal::rdilpool_ge(self, l, r);
}

const RDILExpr* rdil_push(RDILPool* self, const RDILExpr* e) {
    return redasm::api::internal::rdilpool_push(self, e);
}

const RDILExpr* rdil_pop(RDILPool* self, const RDILExpr* e) {
    return redasm::api::internal::rdilpool_pop(self, e);
}

const RDILExpr* rdil_int(RDILPool* self, const RDILExpr* e) {
    return redasm::api::internal::rdilpool_int(self, e);
}

RDILPool* rdillist_getpool(RDILList* self) {
    return redasm::api::internal::rdillist_getpool(self);
}

const RDILExpr* rdillist_at(const RDILList* self, usize idx) {
    return redasm::api::internal::rdillist_at(self, idx);
}

void rdillist_append(RDILList* self, const RDILExpr* e) {
    redasm::api::internal::rdillist_append(self, e);
}

bool rdillist_isempty(const RDILList* self) {
    return redasm::api::internal::rdillist_isempty(self);
}

usize rdillist_getsize(const RDILList* self) {
    return redasm::api::internal::rdillist_getsize(self);
}
