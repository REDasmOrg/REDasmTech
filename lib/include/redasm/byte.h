#pragma once

#include <redasm/common.h>
#include <redasm/types.h>

typedef u32 RDByte;

typedef enum RDByteFlags {
    // Internal Use
    BF_MBYTE = 0x000000ff, // Mask for byte extraction
    BF_MUNKN = 0x8c000000, // Mask for unknown flags
    BF_MCOMM = 0x0001fe00, // Mask for common flags
    BF_MMASK = 0x0001ffff, // Mask for Byte and Common

    // Common Flags (non-volatile)
    BF_BYTE = 1U << 8,      // Indicates the presence of a byte
    BF_NAME = 1U << 9,      // Named entity
    BF_COMMENT = 1U << 10,  // Comment flag
    BF_REFSTO = 1U << 11,   // References to location(s)
    BF_REFSFROM = 1U << 12, // References from location(s)
    BF_SEGMENT = 1U << 13,  // Segment flag
    BF_IMPORT = 1U << 14,   // Import flag
    BF_EXPORT = 1U << 15,   // Export flag
    BF_FUNCTION = 1U << 16, // Function flag

    // Data-Specific Flags
    BF_TYPE = 1U << 17, // Type declaration
    BF_FILL = 1U << 18, // Fill bytes

    // Code-Specific Flags
    BF_CALL = 1U << 19,    // Call instruction
    BF_JUMP = 1U << 20,    // Jump instruction
    BF_FLOW = 1U << 21,    // Flow instruction (ordinary)
    BF_JUMPDST = 1U << 22, // Jump destination
    BF_DSLOT = 1U << 23,   // Instruction has delay slot(s)
    BF_DFLOW = 1U << 24,   // Flow instruction (delay slot)

    // Type Flags (26..27)
    BF_UNKNOWN = 0U << 26, // Unknown type
    BF_DATA = 1U << 26,    // Data type (26)
    BF_CODE = 2U << 26,    // Code type (27)

    // Special Flags
    BF_WEAK = 1U << 28,  // Weak flag
    BF_START = 1U << 29, // Start of Range
    BF_END = 1U << 30,   // End of Range
    BF_CONT = 1U << 31,  // Continuation of Range
} RDByteFlags;

REDASM_EXPORT bool rdbyte_isunknown(RDByte self);
REDASM_EXPORT bool rdbyte_iscode(RDByte self);
REDASM_EXPORT bool rdbyte_isdata(RDByte self);
REDASM_EXPORT bool rdbyte_has(RDByte self, usize f);
REDASM_EXPORT bool rdbyte_hasbyte(RDByte self);
REDASM_EXPORT bool rdbyte_getbyte(RDByte self, u8* b);
