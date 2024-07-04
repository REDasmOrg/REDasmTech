#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

// clang-format off

/*
 * *** RDIL Opcode Table ***
   +---------+-----------+-----------+-----------+----------------------------------+
   | Opcode  | Operand 1 | Operand 2 | Operand 3 |           Description            |
   +---------+-----------+-----------+-----------+----------------------------------+
   | Reg     |    N/A    |    N/A    |    N/A    | (VALUE) Register Name            |
   | Cnst    |    N/A    |    N/A    |    N/A    | (VALUE) Constant Value           |
   | Var     |    N/A    |    N/A    |    N/A    | (VALUE) Symbol Name              |
   | Unknown |           |           |           |                                  |
   | Nop     |           |           |           | nop                              |
   | Add     | left      | right     |           | left + right                     |
   | Sub     | left      | right     |           | left - right                     |
   | Mul     | left      | right     |           | left * right                     |
   | Div     | left      | right     |           | left / right                     |
   | Mod     | left      | right     |           | left % right                     |
   | And     | left      | right     |           | left & right                     |
   | Or      | left      | right     |           | left | right                     |
   | Xor     | left      | right     |           | left ^ right                     |
   | Lsl     | left      | right     |           | left << right                    |
   | Lsr     | left      | right     |           | left >> right                    |
   | Asl     | left      | right     |           | left <<< right                   |
   | Asr     | left      | right     |           | left >>> right                   |
   | Eq      | left      | right     |           | left == right                    |
   | Ne      | left      | right     |           | left != right                    |
   | Lt      | left      | right     |           | left < right                     |
   | Le      | left      | right     |           | left >= right                    |
   | Gt      | left      | right     |           | left > right                     |
   | Ge      | left      | right     |           | left >= right                    |
   | Rol     | left      | right     |           | rotateleft(left, right)          |
   | Ror     | left      | right     |           | rotateright(left, right)         |
   | Not     | u         |           |           | ~u                               |
   | Mem     | u         |           |           | [u]                              |
   | Goto    | u         |           |           | goto(u)                          |
   | Call    | u         |           |           | call(u)                          |
   | Push    | u         |           |           | push(u)                          |
   | Pop     | u         |           |           | u = pop()                        |
   | Copy    | dst       | src       |           | dst = src                        |
   | If      | cond      | t         | f         | if (cond) t else f               |
   | Ret     | cond      |           |           | ret(cond)                        |
   | Int     | u         |           |           | int u                            |
   +---------+-----------+-----------+-----------+----------------------------------+
 */

RD_HANDLE(RDILExpression);
RD_HANDLE(RDILPool);
RD_HANDLE(RDILList);

typedef enum RDILOp {
    RDIL_INVALID = 0,
    RDIL_UNKNOWN,                                         // Special
    RDIL_NOP,                                             // Other
    RDIL_REG, RDIL_CNST, RDIL_VAR,                        // Value
    RDIL_ADD, RDIL_SUB, RDIL_MUL, RDIL_DIV, RDIL_MOD,     // Math
    RDIL_AND, RDIL_OR, RDIL_XOR, RDIL_NOT,                // Logic
    RDIL_LSL, RDIL_LSR, RDIL_ASL, RDIL_ASR,               // Shift
    RDIL_ROL, RDIL_ROR,                                   // Rotate
    RDIL_MEM, RDIL_COPY,                                  // R/W
    RDIL_IF, RDIL_GOTO, RDIL_CALL, RDIL_RET,              // Control Flow
    RDIL_EQ, RDIL_NE, RDIL_LT, RDIL_LE, RDIL_GT, RDIL_GE, // Compare
    RDIL_PUSH, RDIL_POP,                                  // Stack
    RDIL_INT,                                             // Privileged
} RDILOp;
// clang-format on

#define RD_PRIVATE_RDIL_VALUE_FIELDS                                           \
    uptr value;                                                                \
    RDAddress address;                                                         \
    RDOffset offset;                                                           \
    u64 u_value;                                                               \
    i64 s_value;                                                               \
    const char* reg;

typedef struct RDILValue {
    RDILOp type;
    RD_PRIVATE_RDIL_VALUE_FIELDS
} RDILValue;

REDASM_EXPORT const RDILExpression* rdil_unknown(RDILPool* self);
REDASM_EXPORT const RDILExpression* rdil_nop(RDILPool* self);
REDASM_EXPORT const RDILExpression* rdil_var(RDILPool* self, RDAddress address);
REDASM_EXPORT const RDILExpression* rdil_reg(RDILPool* self, const char* reg);
REDASM_EXPORT const RDILExpression* rdil_cnst(RDILPool* self, u64 value);
REDASM_EXPORT const RDILExpression*
rdil_add(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_sub(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_mul(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_div(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_mod(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_and(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_or(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_xor(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_lsl(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_lsr(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_asl(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_asr(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_rol(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_ror(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression* rdil_not(RDILPool* self,
                                             const RDILExpression* e);
REDASM_EXPORT const RDILExpression* rdil_mem(RDILPool* self,
                                             const RDILExpression* e);
REDASM_EXPORT const RDILExpression*
rdil_copy(RDILPool* self, const RDILExpression* dst, const RDILExpression* src);
REDASM_EXPORT const RDILExpression* rdil_goto(RDILPool* self,
                                              const RDILExpression* e);
REDASM_EXPORT const RDILExpression* rdil_call(RDILPool* self,
                                              const RDILExpression* e);
REDASM_EXPORT const RDILExpression* rdil_ret(RDILPool* self,
                                             const RDILExpression* e);
REDASM_EXPORT const RDILExpression* rdil_if(RDILPool* self,
                                            const RDILExpression* cond,
                                            const RDILExpression* t,
                                            const RDILExpression* f);
REDASM_EXPORT const RDILExpression*
rdil_eq(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_ne(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_lt(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_le(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_gt(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression*
rdil_ge(RDILPool* self, const RDILExpression* l, const RDILExpression* r);
REDASM_EXPORT const RDILExpression* rdil_push(RDILPool* self,
                                              const RDILExpression* e);
REDASM_EXPORT const RDILExpression* rdil_pop(RDILPool* self,
                                             const RDILExpression* e);
REDASM_EXPORT const RDILExpression* rdil_int(RDILPool* self,
                                             const RDILExpression* e);

REDASM_EXPORT RDILPool* rdillist_getpool(RDILList* self);

REDASM_EXPORT const RDILExpression* rdillist_at(const RDILList* self,
                                                usize idx);

REDASM_EXPORT void rdillist_append(RDILList* self, const RDILExpression* e);
REDASM_EXPORT bool rdillist_isempty(const RDILList* self);
REDASM_EXPORT usize rdillist_getsize(const RDILList* self);
