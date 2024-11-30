#pragma once

#include <redasm/types.h>
#include <redasm/typing.h>

#define RD_OPERANDCOUNT 6

// Syntatic Sugar
#define foreach_operand(i, op, instr)                                          \
    for(usize i = 0, _break_ = 1u; _break_; _break_ = !_break_)                \
        for(const RDOperand* op = (instr)->operands;                           \
            ((op) < (instr)->operands + RD_OPERANDCOUNT) &&                    \
            ((op)->type != OP_NULL);                                           \
            (op)++, (i)++)

typedef enum RDOperandType {
    OP_NULL = 0,
    OP_REG,
    OP_IMM,
    OP_MEM,    // [Address]
    OP_PHRASE, // [BaseReg + IndexReg]
    OP_DISPL,  // [BaseReg + IndexReg + Displ]
} RDOperandType;

typedef enum RDInstructionFeatures {
    INSTR_NONE = 0,
    INSTR_STOP = (1 << 0),
    INSTR_JUMP = (1 << 1),
    INSTR_CALL = (1 << 2),
} RDInstructionFeatures;

typedef struct _RDPhraseOperand {
    usize base;
    usize index;
} _RDPhraseOperand;

typedef struct _RDDisplOperand {
    usize base;
    usize index;
    usize scale;
    usize displ;
} _RDDisplOperand;

typedef struct RDOperand {
    usize type;
    usize features;
    RDType dtype;

    union {
        usize reg;
        usize imm;
        RDAddress mem;
        _RDPhraseOperand phrase;
        _RDDisplOperand displ;
    };
} RDOperand;

typedef struct RDInstruction {
    RDAddress address;
    usize id;
    usize type;
    usize features;
    usize length;
    RDOperand operands[RD_OPERANDCOUNT];

    union {
        void* userdata;
        uptr uservalue;
    };
} RDInstruction;

inline usize rdinstruction_operandscount(const RDInstruction* instr) {
    int c = 0;
    while(instr->operands[c].type != OP_NULL)
        c++;
    return c;
}
