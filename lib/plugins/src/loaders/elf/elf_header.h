#pragma once

#include <redasm/redasm.h>

/*
 * FROM: https://man7.org/linux/man-pages/man5/elf.5.html
 *
 * The following types are used for N-bit architectures (N=32,64, ElfN
 * stands for Elf32 or Elf64, uintN_t stands for uint32_t or uint64_t):

 * ElfN_Addr       Unsigned program address, uintN_t
 * ElfN_Off        Unsigned file offset, uintN_t
 * ElfN_Section    Unsigned section index, uint16_t
 * ElfN_Versym     Unsigned version symbol information, uint16_t
 * Elf_Byte        unsigned char
 * ElfN_Half       uint16_t
 * ElfN_Sword      int32_t
 * ElfN_Word       uint32_t
 * ElfN_Sxword     int64_t
 * ElfN_Xword      uint64_t
 */

// clang-format off
template<int bits> struct ElfUnsignedT { };
template<> struct ElfUnsignedT<32> { using type = u32; };
template<> struct ElfUnsignedT<64> { using type = u64; };

template<int bits> struct ElfSignedT { };
template<> struct ElfSignedT<32> { using type = i32; };
template<> struct ElfSignedT<64> { using type = i64; };
// clang-format on

#pragma pack(push, 1)

struct ElfIdent {
    u8 ei_magic[4];
    u8 ei_class;
    u8 ei_data;
    u8 ei_version;
    u8 ei_osabi;
    u8 ei_abiversion;
    u8 pad[7];
};

template<int bits>
struct ElfEhdr {
    ElfIdent e_ident;
    u16 e_type;
    u16 e_machine;
    u32 e_version;
    typename ElfUnsignedT<bits>::type e_entry;
    typename ElfUnsignedT<bits>::type e_phoff;
    typename ElfUnsignedT<bits>::type e_shoff;
    u32 e_flags;
    u16 e_ehsize;
    u16 e_phentsize;
    u16 e_phnum;
    u16 e_shentsize;
    u16 e_shnum;
    u16 e_shstrndx;
};

struct Elf32Phdr {
    u32 p_type;
    u32 p_offset;
    u32 p_vaddr;
    u32 p_paddr;
    u32 p_filesz;
    u32 p_memsz;
    u32 p_flags;
    u32 p_align;
};

struct Elf64Phdr {
    u32 p_type;
    u32 p_flags;
    u64 p_offset;
    u64 p_vaddr;
    u64 p_paddr;
    u64 p_filesz;
    u64 p_memsz;
    u64 p_align;
};

template<int bits>
using ElfPhdr = std::conditional_t<bits == 64, Elf64Phdr, Elf32Phdr>;

template<int bits>
struct ElfShdr {
    u32 sh_name;
    u32 sh_type;
    typename ElfUnsignedT<bits>::type sh_flags;
    typename ElfUnsignedT<bits>::type sh_addr;
    typename ElfUnsignedT<bits>::type sh_offset;
    typename ElfUnsignedT<bits>::type sh_size;
    u32 sh_link;
    u32 sh_info;
    typename ElfUnsignedT<bits>::type sh_addralign;
    typename ElfUnsignedT<bits>::type sh_entsize;
};

template<int bits>
struct ElfRel {
    typename ElfUnsignedT<bits>::type r_offset;
    typename ElfUnsignedT<bits>::type r_info;
};

template<int bits>
struct ElfRela {
    typename ElfUnsignedT<bits>::type r_offset;
    typename ElfUnsignedT<bits>::type r_info;
    typename ElfSignedT<bits>::type r_addend;
};

struct Elf32Sym {
    u32 st_name;
    u32 st_value;
    u32 st_size;
    u8 st_info;
    u8 st_other;
    u16 st_shndx;
};

struct Elf64Sym {
    u32 st_name;
    u8 st_info;
    u8 st_other;
    u16 st_shndx;
    u64 st_value;
    u64 st_size;
};

template<int bits>
using ElfSym = std::conditional_t<bits == 64, Elf64Sym, Elf32Sym>;

#pragma pack(pop)
