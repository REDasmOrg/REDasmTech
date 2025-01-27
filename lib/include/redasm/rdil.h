#pragma once

#include <redasm/common.h>
#include <redasm/function.h>
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

RD_HANDLE(RDILPool);
RD_HANDLE(RDILList);

typedef enum RDILOp {
    RDIL_INVALID = 0,
    RDIL_UNKNOWN,                                         // Special
    RDIL_NOP,                                             // Other
    RDIL_REG, RDIL_CNST, RDIL_VAR, RDIL_SYM,              // Value
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
    union {                                                                    \
        int reg;                                                               \
        const char* sym;                                                       \
    };

typedef struct RDILValue {
    RDILOp type;
    RD_PRIVATE_RDIL_VALUE_FIELDS
} RDILValue;

typedef struct RDILExpr {
    RDILOp op{RDIL_INVALID};

    union {
        const RDILExpr *n1{nullptr}, *u, *cond;
    };

    union {
        const RDILExpr *n2{nullptr}, *dst, *l, *t;
    };

    union {
        const RDILExpr *n3{nullptr}, *src, *r, *f;
    };

    RD_PRIVATE_RDIL_VALUE_FIELDS
} RDILExpr;

REDASM_EXPORT const RDILExpr* rdil_unknown(RDILPool* self);
REDASM_EXPORT const RDILExpr* rdil_nop(RDILPool* self);
REDASM_EXPORT const RDILExpr* rdil_var(RDILPool* self, RDAddress address);
REDASM_EXPORT const RDILExpr* rdil_reg(RDILPool* self, int reg);
REDASM_EXPORT const RDILExpr* rdil_sym(RDILPool* self, const char* sym);
REDASM_EXPORT const RDILExpr* rdil_cnst(RDILPool* self, u64 value);
REDASM_EXPORT const RDILExpr* rdil_add(RDILPool* self, const RDILExpr* l,
                                       const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_sub(RDILPool* self, const RDILExpr* l,
                                       const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_mul(RDILPool* self, const RDILExpr* l,
                                       const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_div(RDILPool* self, const RDILExpr* l,
                                       const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_mod(RDILPool* self, const RDILExpr* l,
                                       const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_and(RDILPool* self, const RDILExpr* l,
                                       const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_or(RDILPool* self, const RDILExpr* l,
                                      const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_xor(RDILPool* self, const RDILExpr* l,
                                       const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_lsl(RDILPool* self, const RDILExpr* l,
                                       const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_lsr(RDILPool* self, const RDILExpr* l,
                                       const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_asl(RDILPool* self, const RDILExpr* l,
                                       const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_asr(RDILPool* self, const RDILExpr* l,
                                       const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_rol(RDILPool* self, const RDILExpr* l,
                                       const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_ror(RDILPool* self, const RDILExpr* l,
                                       const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_not(RDILPool* self, const RDILExpr* e);
REDASM_EXPORT const RDILExpr* rdil_mem(RDILPool* self, const RDILExpr* e);
REDASM_EXPORT const RDILExpr* rdil_copy(RDILPool* self, const RDILExpr* dst,
                                        const RDILExpr* src);
REDASM_EXPORT const RDILExpr* rdil_goto(RDILPool* self, const RDILExpr* e);
REDASM_EXPORT const RDILExpr* rdil_call(RDILPool* self, const RDILExpr* e);
REDASM_EXPORT const RDILExpr* rdil_ret(RDILPool* self, const RDILExpr* e);
REDASM_EXPORT const RDILExpr* rdil_if(RDILPool* self, const RDILExpr* cond,
                                      const RDILExpr* t, const RDILExpr* f);
REDASM_EXPORT const RDILExpr* rdil_eq(RDILPool* self, const RDILExpr* l,
                                      const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_ne(RDILPool* self, const RDILExpr* l,
                                      const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_lt(RDILPool* self, const RDILExpr* l,
                                      const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_le(RDILPool* self, const RDILExpr* l,
                                      const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_gt(RDILPool* self, const RDILExpr* l,
                                      const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_ge(RDILPool* self, const RDILExpr* l,
                                      const RDILExpr* r);
REDASM_EXPORT const RDILExpr* rdil_push(RDILPool* self, const RDILExpr* e);
REDASM_EXPORT const RDILExpr* rdil_pop(RDILPool* self, const RDILExpr* e);
REDASM_EXPORT const RDILExpr* rdil_int(RDILPool* self, const RDILExpr* e);

REDASM_EXPORT RDILList* rdilist_create();
REDASM_EXPORT RDILPool* rdillist_getpool(RDILList* self);
REDASM_EXPORT void rdil_generate(const RDFunction* f, RDILList* l);

REDASM_EXPORT const RDILExpr* rdillist_at(const RDILList* self, usize idx);

REDASM_EXPORT void rdillist_append(RDILList* self, const RDILExpr* e);
REDASM_EXPORT bool rdillist_isempty(const RDILList* self);
REDASM_EXPORT usize rdillist_getsize(const RDILList* self);
