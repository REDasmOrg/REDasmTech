#pragma once

#include <redasm/redasm.h>
#include <string>

constexpr u8 DW_EH_PE_PTR = 0x00;
constexpr u8 DW_EH_PE_ULEB128 = 0x01;
constexpr u8 DW_EH_PE_UDATA2 = 0x02;
constexpr u8 DW_EH_PE_UDATA4 = 0x03;
constexpr u8 DW_EH_PE_UDATA8 = 0x04;
constexpr u8 DW_EH_PE_SLEB128 = 0x09;
constexpr u8 DW_EH_PE_SDATA2 = 0x0A;
constexpr u8 DW_EH_PE_SDATA4 = 0x0B;
constexpr u8 DW_EH_PE_SDATA8 = 0x0C;

constexpr u8 DW_EH_PE_ABSPTR = 0x00;
constexpr u8 DW_EH_PE_PCREL = 0x10;
constexpr u8 DW_EH_PE_DATAREL = 0x30;
constexpr u8 DW_EH_PE_INDIRECT = 0x80;

constexpr u8 DW_EH_PE_OMIT = 0xFF;

#pragma pack(push, 1)

struct ElfEhFrameHdr {
    u8 version;          // Must be 1
    u8 eh_frame_ptr_enc; // Encoding of eh_frame_ptr (DW_EH_PE_xxx)
    u8 fde_count_enc;    // Encoding of FDE count
    u8 table_enc;        // Encoding of the binary search table entries
    // encoded_pointer eh_frame_ptr; // Encoded pointer to .eh_frame
    // encoded_value fde_count;      // Number of FDE entries
    // table_entry search_table[];   // Sorted table: PC start -> FDE pointer
};

namespace elf_dwarf {

struct CIEInfo {
    std::string aug_string;
    u8 fde_ptr_encoding;
};

const char* get_type(u8 dwarftype);
usize apply_type(RDAddress addr, u8 dwarftype, RDValue* v = nullptr);

RDAddress calc_address(RDAddress pcrel, RDAddress datarel, const RDValue& val,
                       u8 tableenc);

} // namespace elf_dwarf

#pragma pack(pop)
