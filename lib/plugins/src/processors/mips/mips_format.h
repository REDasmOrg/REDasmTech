#pragma once

#include "mips_instruction.h"
#include <array>
#include <redasm/redasm.h>
#include <string_view>
#include <unordered_map>

enum MIPSVersion { MIPS_VERSION_NONE, MIPS_VERSION_I };

enum MIPSEncoding {
    MIPS_ENCODING_NONE,
    MIPS_ENCODING_R,
    MIPS_ENCODING_C,
    MIPS_ENCODING_I,
    MIPS_ENCODING_J,
    MIPS_ENCODING_B,
    MIPS_ENCODING_C0,
    MIPS_ENCODING_C1,
    MIPS_ENCODING_C2,
    MIPS_ENCODING_CLS,
    MIPS_ENCODING_COUNT
};

enum MIPSCategory {
    MIPS_CATEGORY_NONE,
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
    int encoding;
    u32 version;
};

union MIPSMacroOpCode {
    struct {
        unsigned reg : 5;
        union {
            RDAddress address;
            u64 u_value;
            i64 s_value;
        };
    } regimm; // opcode reg, imm

    struct {
        union {
            RDAddress address;
            u64 u_value;
            i64 s_value;
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
using MIPSMacroMap = std::unordered_map<std::string_view, MIPSMacro>;

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
