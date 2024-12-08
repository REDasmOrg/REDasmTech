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

    OP_USERBASE, // User defined operands
} RDOperandType;

typedef enum RDInstructionFeatures {
    INSTR_NONE = 0,
    INSTR_STOP = (1 << 0),
    INSTR_JUMP = (1 << 1),
    INSTR_CALL = (1 << 2),
} RDInstructionFeatures;

typedef usize RDRegister;

typedef struct _RDPhraseOperand {
    RDRegister base;
    RDRegister index;
} _RDPhraseOperand;

typedef struct _RDDisplOperand {
    RDRegister base;
    RDRegister index;

    union {
        u64 displ;
        i64 s_displ;
    };

    int scale;
} _RDDisplOperand;

typedef struct _RDUserOperand {
    RDRegister reg1;
    RDRegister reg2;

    union {
        usize val;
        isize s_val;
    };
} _RDUserOperand;

typedef struct RDOperand {
    usize type;
    usize features;
    RDType dtype;

    union {
        RDRegister reg;
        u64 imm;
        i64 s_imm;
        RDAddress mem;
        _RDPhraseOperand phrase;
        _RDDisplOperand displ;
        _RDUserOperand user;
    };

    u32 userdata1;
    u32 userdata2;
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
