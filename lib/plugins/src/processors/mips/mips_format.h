#pragma once

#include "mips_instruction.h"
#include <array>
#include <redasm/redasm.h>
#include <unordered_map>

enum MIPSVersion { MIPS_VERSION_NONE, MIPS_VERSION_I };

enum MIPSFormat {
    MIPS_FORMAT_NONE = 0,
    MIPS_FORMAT_R,
    MIPS_FORMAT_C,
    MIPS_FORMAT_I,
    MIPS_FORMAT_J,
    MIPS_FORMAT_B,
    MIPS_FORMAT_C0,
    MIPS_FORMAT_C1,
    MIPS_FORMAT_C2,
    MIPS_FORMAT_CLS,
    MIPS_FORMAT_COUNT
};

enum MIPSCategory {
    MIPS_CATEGORY_NONE = 0,
    MIPS_CATEGORY_MACRO,

    MIPS_CATEGORY_LOAD,
    MIPS_CATEGORY_STORE,
    MIPS_CATEGORY_JUMP,
    MIPS_CATEGORY_JUMPCOND,
    MIPS_CATEGORY_CALL,
    MIPS_CATEGORY_RET,
};

struct MIPSOpcode {
    u32 id;
    int category;
    int format;
    u32 version;
};

union MIPSMacroOpCode {
    struct {
        unsigned reg : 5;
        union {
            RDAddress address;
            u64 u_imm64;
            i64 s_imm64;
            u32 u_imm32;
            i32 s_imm32;
            u16 u_imm16;
            i16 s_imm16;
        };
    } regimm; // opcode reg, imm

    struct {
        union {
            RDAddress address;
            u64 u_imm64;
            i64 s_imm64;
            u32 u_imm32;
            i32 s_imm32;
            u16 u_imm16;
            i16 s_imm16;
        };
        unsigned base : 5;
        unsigned rt : 5;
    } loadstore; // opcode reg, offset(base)
};

struct MIPSDecodedInstruction {
    MIPSInstruction instr;
    const MIPSOpcode* opcode;
    MIPSMacroOpCode macro;
    int length{sizeof(MIPSInstruction)};
};

using MIPSOpcodeArray = std::array<MIPSOpcode, 1 << MIPS_OP_BITS>;
using MIPSMacro = std::pair<MIPSOpcode, size_t>;
using MIPSMacroMap = std::unordered_map<MIPSInstructionId, MIPSMacro>;

extern const MIPSMacroMap MIPS_OPCODES_MACRO;
extern MIPSOpcodeArray mips_opcodes_r;
extern MIPSOpcodeArray mips_opcodes_c;
extern MIPSOpcodeArray mips_opcodes_i;
extern MIPSOpcodeArray mips_opcodes_j;
extern MIPSOpcodeArray mips_opcodes_b;
extern MIPSOpcodeArray mips_opcodes_c0;
extern MIPSOpcodeArray mips_opcodes_c1;
extern MIPSOpcodeArray mips_opcodes_c2;
extern MIPSOpcodeArray mips_opcodes_cls;

void mips_initialize_formats();
