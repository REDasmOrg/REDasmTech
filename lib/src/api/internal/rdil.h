#include <redasm/rdil.h>

namespace redasm::api::internal {

// clang-format off
const RDILExpression* rdiltree_unknown(RDILExpressionTree* self);
const RDILExpression* rdiltree_nop(RDILExpressionTree* self);
const RDILExpression* rdiltree_var(RDILExpressionTree* self, RDAddress address);
const RDILExpression* rdiltree_reg(RDILExpressionTree* self, const char* reg);
const RDILExpression* rdiltree_cnst(RDILExpressionTree* self, u64 value);
const RDILExpression* rdiltree_add(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_sub(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_mul(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_div(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_mod(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_and(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_or(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_xor(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_lsl(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_lsr(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_asl(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_asr(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_rol(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_ror(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_not(RDILExpressionTree* self, const RDILExpression* e);
const RDILExpression* rdiltree_mem(RDILExpressionTree* self, const RDILExpression* e);
const RDILExpression* rdiltree_copy(RDILExpressionTree* self, const RDILExpression* dst, const RDILExpression* src);
const RDILExpression* rdiltree_goto(RDILExpressionTree* self, const RDILExpression* e);
const RDILExpression* rdiltree_call(RDILExpressionTree* self, const RDILExpression* e);
const RDILExpression* rdiltree_ret(RDILExpressionTree* self, const RDILExpression* e);
const RDILExpression* rdiltree_if(RDILExpressionTree* self, const RDILExpression* cond, const RDILExpression* t, const RDILExpression* f);
const RDILExpression* rdiltree_eq(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_ne(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_lt(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_le(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_gt(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_ge(RDILExpressionTree* self, const RDILExpression* l, const RDILExpression* r);
const RDILExpression* rdiltree_push(RDILExpressionTree* self, const RDILExpression* e);
const RDILExpression* rdiltree_pop(RDILExpressionTree* self, const RDILExpression* e);
const RDILExpression* rdiltree_int(RDILExpressionTree* self, const RDILExpression* e);
// clang-format on

} // namespace redasm::api::internal
