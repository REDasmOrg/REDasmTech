#include <redasm/rdil.h>

namespace redasm::api::internal {

// clang-format off
const RDILExpression* rdilpool_unknown(RDILPool* self);
const RDILExpression* rdilpool_nop(RDILPool* self);
const RDILExpression* rdilpool_var(RDILPool* self, RDAddress address);
const RDILExpression* rdilpool_reg(RDILPool* self, const char* reg);
const RDILExpression* rdilpool_cnst(RDILPool* self, u64 value);
const RDILExpression* rdilpool_add(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_sub(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_mul(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_div(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_mod(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_and(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_or(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_xor(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_lsl(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_lsr(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_asl(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_asr(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_rol(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_ror(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_not(RDILPool* self, const RDILExpression* e);
const RDILExpression* rdilpool_mem(RDILPool* self, const RDILExpression* e);
const RDILExpression* rdilpool_copy(RDILPool* self, const RDILExpression* dst, const RDILExpression* src);
const RDILExpression* rdilpool_goto(RDILPool* self, const RDILExpression* e);
const RDILExpression* rdilpool_call(RDILPool* self, const RDILExpression* e);
const RDILExpression* rdilpool_ret(RDILPool* self, const RDILExpression* e);
const RDILExpression* rdilpool_if(RDILPool* self, const RDILExpression* cond, const RDILExpression* t, const RDILExpression* f);
const RDILExpression* rdilpool_eq(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_ne(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_lt(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_le(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_gt(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_ge(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_push(RDILPool* self, const RDILExpression* e);
const RDILExpression* rdilpool_pop(RDILPool* self, const RDILExpression* e);
const RDILExpression* rdilpool_int(RDILPool* self, const RDILExpression* e);
// clang-format on

RDILPool* rdillist_getpool(RDILList* self);
const RDILExpression* rdillist_at(const RDILList* self, usize idx);
void rdillist_append(RDILList* self, const RDILExpression* e);
bool rdillist_isempty(const RDILList* self);
usize rdillist_getsize(const RDILList* self);

} // namespace redasm::api::internal
