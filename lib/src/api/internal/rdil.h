#include <redasm/rdil.h>

namespace redasm::api::internal {

// clang-format off
const RDILExpr* rdilpool_unknown(RDILPool* self);
const RDILExpr* rdilpool_nop(RDILPool* self);
const RDILExpr* rdilpool_var(RDILPool* self, RDAddress address);
const RDILExpr* rdilpool_reg(RDILPool* self, int reg);
const RDILExpr* rdilpool_sym(RDILPool* self, const char* sym);
const RDILExpr* rdilpool_cnst(RDILPool* self, u64 value);
const RDILExpr* rdilpool_add(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_sub(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_mul(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_div(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_mod(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_and(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_or(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_xor(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_lsl(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_lsr(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_asl(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_asr(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_rol(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_ror(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_not(RDILPool* self, const RDILExpr* e);
const RDILExpr* rdilpool_mem(RDILPool* self, const RDILExpr* e);
const RDILExpr* rdilpool_copy(RDILPool* self, const RDILExpr* dst, const RDILExpr* src);
const RDILExpr* rdilpool_goto(RDILPool* self, const RDILExpr* e);
const RDILExpr* rdilpool_call(RDILPool* self, const RDILExpr* e);
const RDILExpr* rdilpool_ret(RDILPool* self, const RDILExpr* e);
const RDILExpr* rdilpool_if(RDILPool* self, const RDILExpr* cond, const RDILExpr* t, const RDILExpr* f);
const RDILExpr* rdilpool_eq(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_ne(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_lt(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_le(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_gt(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_ge(RDILPool* self, const RDILExpr* l, const RDILExpr* r);
const RDILExpr* rdilpool_push(RDILPool* self, const RDILExpr* e);
const RDILExpr* rdilpool_pop(RDILPool* self, const RDILExpr* e);
const RDILExpr* rdilpool_int(RDILPool* self, const RDILExpr* e);
// clang-format on

RDILPool* rdillist_getpool(RDILList* self);
const RDILExpr* rdillist_at(const RDILList* self, usize idx);
void rdillist_append(RDILList* self, const RDILExpr* e);
bool rdillist_isempty(const RDILList* self);
usize rdillist_getsize(const RDILList* self);

} // namespace redasm::api::internal
