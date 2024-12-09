#pragma once

#include <array>
#include <string_view>

enum MIPSRegisters {
    MIPS_REG_ZERO,
    MIPS_REG_AT,
    MIPS_REG_V0,
    MIPS_REG_V1,
    MIPS_REG_A0,
    MIPS_REG_A1,
    MIPS_REG_A2,
    MIPS_REG_A3,
    MIPS_REG_T0,
    MIPS_REG_T1,
    MIPS_REG_T2,
    MIPS_REG_T3,
    MIPS_REG_T4,
    MIPS_REG_T5,
    MIPS_REG_T6,
    MIPS_REG_T7,
    MIPS_REG_S0,
    MIPS_REG_S1,
    MIPS_REG_S2,
    MIPS_REG_S3,
    MIPS_REG_S4,
    MIPS_REG_S5,
    MIPS_REG_S6,
    MIPS_REG_S7,
    MIPS_REG_T8,
    MIPS_REG_T9,
    MIPS_REG_K0,
    MIPS_REG_K1,
    MIPS_REG_GP,
    MIPS_REG_SP,
    MIPS_REG_FP,
    MIPS_REG_RA,

    MIPS_REG_MAX,
};

enum MIPSCOP0Registers {
    MIPS_REG_COP0_INDEX,
    MIPS_REG_COP0_RANDOM,
    MIPS_REG_COP0_ENTRY_LO0,
    MIPS_REG_COP0_ENTRY_LO1,
    MIPS_REG_COP0_CONTEXT,
    MIPS_REG_COP0_PAGE_MASK,
    MIPS_REG_COP0_WIRED,
    MIPS_REG_COP0_RESERVED,
    MIPS_REG_COP0_BAD_V_ADDR,
    MIPS_REG_COP0_COUNT,
    MIPS_REG_COP0_ENTRY_HI,
    MIPS_REG_COP0_COMPARE,
    MIPS_REG_COP0_STATUS,
    MIPS_REG_COP0_CAUSE,
    MIPS_REG_COP0_EPC,
    MIPS_REG_COP0_PR_ID,
    MIPS_REG_COP0_CONFIG,
    MIPS_REG_COP0_LL_ADDR,
    MIPS_REG_COP0_WATCH_LO,
    MIPS_REG_COP0_WATCH_HI,
    MIPS_REG_COP0_X_CONTEXT,

    MIPS_REG_COP0_CACHE_ERR = 27,
    MIPS_REG_COP0_TAG_LO,
    MIPS_REG_CO_P0_TAG_HI,
    MIPS_REG_COP0_ERROR_EPC,

    MIPS_REG_COP0_MAX = 32,
};

constexpr std::array<std::string_view, MIPS_REG_MAX> GPR_REGS = {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
    "$t0",   "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0",   "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8",   "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra",
};

constexpr std::array<std::string_view, MIPS_REG_COP0_MAX> COP0_REGS = {
    "$Index",    "$Random",   "$EntryLo0", "$EntryLo1", "$Context", "$PageMask",
    "$Wired",    "$Reserved", "$BadVAddr", "$Count",    "$EntryHi", "$Compare",
    "$Status",   "$Cause",    "$EPC",      "PRId",      "$Config",  "$LLAddr",
    "$WatchLo",  "$WatchHi",  "$XContext", "$21",       "$22",      "$23",
    "$24",       "$25",       "$26",       "$CacheErr", "$TagLo",   "$TagHi",
    "$ErrorEPC", "$31",
};
