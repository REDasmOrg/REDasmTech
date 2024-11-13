#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

typedef u32 RDByte;

typedef enum RDByteFlags {
    // Internal Use
    BF_MBYTE = 0x000000ff, // Mask for byte extraction
    BF_MUNKN = 0x60000000, // Mask for unknown flags
    BF_MMASK = 0x0003ffff, // Mask for Byte and Common

    // Common Flags
    BF_HASBYTE = 1U << 8,   // Indicates the presence of a byte
    BF_WEAK = 1U << 9,      // Weak reference
    BF_NAME = 1U << 10,     // Named entity
    BF_COMMENT = 1U << 11,  // Comment flag
    BF_REFS = 1U << 12,     // References from location
    BF_REFSTO = 1U << 13,   // References to location
    BF_SEGMENT = 1U << 14,  // Segment flag
    BF_IMPORT = 1U << 15,   // Import flag
    BF_EXPORT = 1U << 16,   // Export flag
    BF_FUNCTION = 1U << 17, // Function flag

    // Type Flags (29..30)
    BF_UNKNOWN = 0U << 29, // Unknown type
    BF_DATA = 1U << 29,    // Data type
    BF_CODE = 2U << 29,    // Code type

    // Data-Specific Flags
    BF_TYPE = BF_DATA | (1U << 18),  // Data type flag
    BF_ARRAY = BF_DATA | (1U << 19), // Array type flag

    // Code-Specific Flags
    BF_INSTR = BF_CODE | (1U << 18),           // Instruction
    BF_CALL = BF_CODE | BF_INSTR | (1U << 19), // Call instruction
    BF_JUMP = BF_CODE | BF_INSTR | (1U << 20), // Jump instruction
    BF_FLOW = BF_CODE | BF_INSTR | (1U << 21), // Flow control instruction
    BF_JUMPDST = 1U << 22,                     // Jump destination

    // Special Flags
    BF_CONT = 1U << 31, // Continuation flag
} RDByteFlags;

REDASM_EXPORT bool rdbyte_isunknown(RDByte self);
REDASM_EXPORT bool rdbyte_iscode(RDByte self);
REDASM_EXPORT bool rdbyte_isdata(RDByte self);
REDASM_EXPORT bool rdbyte_has(RDByte self, usize f);
REDASM_EXPORT bool rdbyte_hasbyte(RDByte self);
REDASM_EXPORT bool rdbyte_getbyte(RDByte self, u8* b);
