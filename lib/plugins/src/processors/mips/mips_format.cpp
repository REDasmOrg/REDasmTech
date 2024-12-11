#include "mips_format.h"

#define MIPS_MACRO(name, type, size)                                           \
    {                                                                          \
        name,                                                                  \
        {                                                                      \
            {                                                                  \
                type,                                                          \
                MIPS_CATEGORY_MACRO,                                           \
                MIPS_ENCODING_NONE,                                            \
                MIPS_VERSION_NONE,                                             \
            },                                                                 \
            size,                                                              \
        },                                                                     \
    }

MIPSOpcodeArray mips_opcodes_i{};
MIPSOpcodeArray mips_opcodes_c{};
MIPSOpcodeArray mips_opcodes_r{};
MIPSOpcodeArray mips_opcodes_j{};
MIPSOpcodeArray mips_opcodes_b{};
MIPSOpcodeArray mips_opcodes_c0{};
MIPSOpcodeArray mips_opcodes_c1{};
MIPSOpcodeArray mips_opcodes_c2{};
MIPSOpcodeArray mips_opcodes_cls{};

const MIPSMacroMap MIPS_OPCODES_MACRO = {
    MIPS_MACRO("la", MIPS_MACRO_LA, sizeof(MIPSInstruction) * 2),
    MIPS_MACRO("lw", MIPS_MACRO_LW, sizeof(MIPSInstruction) * 2),
    MIPS_MACRO("lhu", MIPS_MACRO_LHU, sizeof(MIPSInstruction) * 2),
    MIPS_MACRO("sw", MIPS_MACRO_SW, sizeof(MIPSInstruction) * 2),
    MIPS_MACRO("sh", MIPS_MACRO_SH, sizeof(MIPSInstruction) * 2),

    MIPS_MACRO("li", MIPS_MACRO_LI, sizeof(MIPSInstruction)),
    MIPS_MACRO("b", MIPS_MACRO_B, sizeof(MIPSInstruction)),
    MIPS_MACRO("nop", MIPS_MACRO_NOP, sizeof(MIPSInstruction)),
    MIPS_MACRO("move", MIPS_MACRO_MOVE, sizeof(MIPSInstruction)),
};

void mips_initialize_formats() {
    // clang-format off
    mips_opcodes_r[0b100000] = {MIPS_INSTR_ADD, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b100001] = {MIPS_INSTR_ADDU, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b100100] = {MIPS_INSTR_AND, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b011010] = {MIPS_INSTR_DIV, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b011011] = {MIPS_INSTR_DIVU, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b011000] = {MIPS_INSTR_MULT, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b011001] = {MIPS_INSTR_MULTU, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b100111] = {MIPS_INSTR_NOR, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b100101] = {MIPS_INSTR_OR, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b000000] = {MIPS_INSTR_SLL, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b000011] = {MIPS_INSTR_SRA, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b000010] = {MIPS_INSTR_SRL, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b100010] = {MIPS_INSTR_SUB, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b100011] = {MIPS_INSTR_SUBU, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b100110] = {MIPS_INSTR_XOR, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b101010] = {MIPS_INSTR_SLT, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b101011] = {MIPS_INSTR_SLTU, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b001000] = {MIPS_INSTR_JR, MIPS_CATEGORY_JUMP, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b010000] = {MIPS_INSTR_MFHI, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b010010] = {MIPS_INSTR_MFLO, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b010001] = {MIPS_INSTR_MTHI, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b010011] = {MIPS_INSTR_MTLO, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b000100] = {MIPS_INSTR_SLLV, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b000111] = {MIPS_INSTR_SRAV, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b000110] = {MIPS_INSTR_SRLV, MIPS_CATEGORY_NONE, MIPS_ENCODING_R, MIPS_VERSION_I };
    mips_opcodes_r[0b001001] = {MIPS_INSTR_JALR, MIPS_CATEGORY_CALL, MIPS_ENCODING_R, MIPS_VERSION_I };

    mips_opcodes_c[0b110000] = {MIPS_INSTR_TGE, MIPS_CATEGORY_NONE, MIPS_ENCODING_C, MIPS_VERSION_I };
    mips_opcodes_c[0b110100] = {MIPS_INSTR_TEQ, MIPS_CATEGORY_NONE, MIPS_ENCODING_C, MIPS_VERSION_I };

    mips_opcodes_i[0b001000] = {MIPS_INSTR_ADDI, MIPS_CATEGORY_NONE, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b001001] = {MIPS_INSTR_ADDIU, MIPS_CATEGORY_NONE, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b001100] = {MIPS_INSTR_ANDI, MIPS_CATEGORY_NONE, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b001101] = {MIPS_INSTR_ORI, MIPS_CATEGORY_NONE, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b001111] = {MIPS_INSTR_LUI, MIPS_CATEGORY_LOAD, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b000100] = {MIPS_INSTR_BEQ, MIPS_CATEGORY_JUMPCOND, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b000101] = {MIPS_INSTR_BNE, MIPS_CATEGORY_JUMPCOND, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b000001] = {MIPS_INSTR_BGEZ, MIPS_CATEGORY_JUMPCOND, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b000111] = {MIPS_INSTR_BGTZ, MIPS_CATEGORY_JUMPCOND, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b000110] = {MIPS_INSTR_BLEZ, MIPS_CATEGORY_JUMPCOND, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b100000] = {MIPS_INSTR_LB, MIPS_CATEGORY_LOAD, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b100100] = {MIPS_INSTR_LBU, MIPS_CATEGORY_LOAD, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b100001] = {MIPS_INSTR_LH, MIPS_CATEGORY_LOAD, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b100101] = {MIPS_INSTR_LHU, MIPS_CATEGORY_LOAD, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b100011] = {MIPS_INSTR_LW, MIPS_CATEGORY_LOAD, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b100010] = {MIPS_INSTR_LWL, MIPS_CATEGORY_LOAD, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b100110] = {MIPS_INSTR_LWR, MIPS_CATEGORY_LOAD, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b101000] = {MIPS_INSTR_SB, MIPS_CATEGORY_STORE, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b101001] = {MIPS_INSTR_SH, MIPS_CATEGORY_STORE, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b101011] = {MIPS_INSTR_SW, MIPS_CATEGORY_STORE, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b101010] = {MIPS_INSTR_SWL, MIPS_CATEGORY_STORE, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b101110] = {MIPS_INSTR_SWR, MIPS_CATEGORY_STORE, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b011001] = {MIPS_INSTR_LHI, MIPS_CATEGORY_NONE, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b011000] = {MIPS_INSTR_LLO, MIPS_CATEGORY_NONE, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b001010] = {MIPS_INSTR_SLTI, MIPS_CATEGORY_NONE, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b001011] = {MIPS_INSTR_SLTIU, MIPS_CATEGORY_NONE, MIPS_ENCODING_I, MIPS_VERSION_I };
    mips_opcodes_i[0b001110] = {MIPS_INSTR_XORI, MIPS_CATEGORY_NONE, MIPS_ENCODING_I, MIPS_VERSION_I };

    mips_opcodes_j[0b000010] = {MIPS_INSTR_J, MIPS_CATEGORY_JUMP, MIPS_ENCODING_J, MIPS_VERSION_I };
    mips_opcodes_j[0b000011] = {MIPS_INSTR_JAL, MIPS_CATEGORY_CALL, MIPS_ENCODING_J, MIPS_VERSION_I };

    mips_opcodes_b[0b001100] = {MIPS_INSTR_SYSCALL, MIPS_CATEGORY_NONE, MIPS_ENCODING_B, MIPS_VERSION_I };
    mips_opcodes_b[0b001101] = {MIPS_INSTR_BREAK, MIPS_CATEGORY_NONE, MIPS_ENCODING_B, MIPS_VERSION_I };

    mips_opcodes_c0[0b00000] = {MIPS_INSTR_MFC0, MIPS_CATEGORY_LOAD, MIPS_ENCODING_C0, MIPS_VERSION_I };
    mips_opcodes_c0[0b00100] = {MIPS_INSTR_MTC0, MIPS_CATEGORY_LOAD, MIPS_ENCODING_C0, MIPS_VERSION_I };

    mips_opcodes_c2[0b00000] = {MIPS_INSTR_MFC2, MIPS_CATEGORY_NONE, MIPS_ENCODING_C2, MIPS_VERSION_I };
    mips_opcodes_c2[0b00100] = {MIPS_INSTR_MTC2, MIPS_CATEGORY_NONE, MIPS_ENCODING_C2, MIPS_VERSION_I };
    mips_opcodes_c2[0b00010] = {MIPS_INSTR_CFC2, MIPS_CATEGORY_NONE, MIPS_ENCODING_C2, MIPS_VERSION_I };
    mips_opcodes_c2[0b00110] = {MIPS_INSTR_CTC2, MIPS_CATEGORY_NONE, MIPS_ENCODING_C2, MIPS_VERSION_I };

    mips_opcodes_cls[0b110001] = {MIPS_INSTR_LWC1, MIPS_CATEGORY_NONE, MIPS_ENCODING_CLS, MIPS_VERSION_I };
    mips_opcodes_cls[0b111001] = {MIPS_INSTR_SWC1, MIPS_CATEGORY_NONE, MIPS_ENCODING_CLS, MIPS_VERSION_I };
    mips_opcodes_cls[0b110010] = {MIPS_INSTR_LWC2, MIPS_CATEGORY_NONE, MIPS_ENCODING_CLS, MIPS_VERSION_I };
    mips_opcodes_cls[0b111010] = {MIPS_INSTR_LWC2, MIPS_CATEGORY_NONE, MIPS_ENCODING_CLS, MIPS_VERSION_I };
    // clang-format on
}
