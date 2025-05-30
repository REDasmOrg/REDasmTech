#pragma once

#include <redasm/types.h>
#include <redasm/typing.h>

#define RD_NOPERANDS 8
#define RD_NMNEMONIC 32

// Syntatic Sugar
#define foreach_operand(i, op, instr)                                          \
    for(int i = 0, _break_ = 1; _break_; _break_ = !_break_)                   \
        for(const RDOperand* op = (instr)->operands;                           \
            ((op) < (instr)->operands + RD_NOPERANDS) &&                       \
            ((op)->type != OP_NULL);                                           \
            (op)++, (i)++)

typedef enum RDOperandType {
    OP_NULL = 0,
    OP_REG,
    OP_IMM,
    OP_ADDR,   // Address immediate
    OP_MEM,    // [Address]
    OP_PHRASE, // [BaseReg + IndexReg]
    OP_DISPL,  // [BaseReg + IndexReg + Displ]

    OP_USERBASE, // User defined operands
} RDOperandType;

typedef enum RDInstructionFeatures {
    IF_NONE = 0,
    IF_STOP = (1 << 0),  // Stops flow
    IF_JUMP = (1 << 1),  // Can branch
    IF_CALL = (1 << 2),  // Call a function
    IF_NOP = (1 << 3),   // No-Operation
    IF_DSLOT = (1 << 4), // Is a delay slot
} RDInstructionFeatures;

typedef struct _RDPhraseOperand {
    int base;
    int index;
} _RDPhraseOperand;

typedef struct _RDDisplOperand {
    int base;
    int index;

    union {
        u64 displ;
        i64 s_displ;
    };

    int scale;
} _RDDisplOperand;

typedef struct _RDUserOperand {
    union {
        int reg1;
        usize val1;
        isize s_val1;
    };

    union {
        int reg2;
        usize val2;
        isize s_val2;
    };
    union {
        int reg3;
        usize val3;
        isize s_val3;
    };
} _RDUserOperand;

typedef struct RDOperand {
    usize type;
    RDType dtype;

    union {
        int reg;
        u64 addr;
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
    u32 id;
    u16 length;
    u8 features;
    u8 delayslots;
    RDOperand operands[RD_NOPERANDS];

    union {
        void* userdata1;
        uptr uservalue1;
    };

    union {
        void* userdata2;
        uptr uservalue2;
        char mnemonic[RD_NMNEMONIC];
    };
} RDInstruction;

REDASM_EXPORT void rdinstruction_setmnemonic(RDInstruction* instr,
                                             const char* mnemonic);
REDASM_EXPORT usize rdinstruction_operandscount(const RDInstruction* instr);
