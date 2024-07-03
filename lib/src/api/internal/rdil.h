#include <redasm/rdil.h>

namespace redasm::api::internal {

// clang-format off
const RDILExpression* rdilpool_unknown(RDILExpressionPool* self);
const RDILExpression* rdilpool_nop(RDILExpressionPool* self);
const RDILExpression* rdilpool_var(RDILExpressionPool* self, RDAddress address);
const RDILExpression* rdilpool_reg(RDILExpressionPool* self, const char* reg);
const RDILExpression* rdilpool_cnst(RDILExpressionPool* self, u64 value);
const RDILExpression* rdilpool_add(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_sub(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_mul(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_div(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_mod(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_and(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_or(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_xor(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_lsl(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_lsr(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_asl(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_asr(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_rol(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_ror(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_not(RDILExpressionPool* self, const RDILExpression* e);
const RDILExpression* rdilpool_mem(RDILExpressionPool* self, const RDILExpression* e);
const RDILExpression* rdilpool_copy(RDILExpressionPool* self, const RDILExpression* dst, const RDILExpression* src);
const RDILExpression* rdilpool_goto(RDILExpressionPool* self, const RDILExpression* e);
const RDILExpression* rdilpool_call(RDILExpressionPool* self, const RDILExpression* e);
const RDILExpression* rdilpool_ret(RDILExpressionPool* self, const RDILExpression* e);
const RDILExpression* rdilpool_if(RDILExpressionPool* self, const RDILExpression* cond, const RDILExpression* t, const RDILExpression* f);
const RDILExpression* rdilpool_eq(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_ne(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_lt(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_le(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_gt(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_ge(RDILExpressionPool* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdilpool_push(RDILExpressionPool* self, const RDILExpression* e);
const RDILExpression* rdilpool_pop(RDILExpressionPool* self, const RDILExpression* e);
const RDILExpression* rdilpool_int(RDILExpressionPool* self, const RDILExpression* e);
// clang-format on

RDILExpressionPool* rdillist_getpool(RDILExpressionList* self);
const RDILExpression* rdillist_at(const RDILExpressionList* self, usize idx);
void rdillist_append(RDILExpressionList* self, const RDILExpression* e);
bool rdillist_isempty(const RDILExpressionList* self);
usize rdillist_getsize(const RDILExpressionList* self);

} // namespace redasm::api::internal
