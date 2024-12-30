#include "elf_types.h"

namespace elf_types {

namespace {

const RDStructField ELF_EHDR_32LE[] = {
    {"u8[16]", "e_ident"},  {"u16", "e_type"},     {"u16", "e_machine"},
    {"u32", "e_version"},   {"u32", "e_entry"},    {"u32", "e_phoff"},
    {"u32", "e_shoff"},     {"u32", "e_flags"},    {"u16", "e_ehsize"},
    {"u16", "e_phentsize"}, {"u16", "e_phnum"},    {"u16", "e_shentsize"},
    {"u16", "e_shnum"},     {"u16", "e_shstrndx"}, {nullptr, nullptr},
};

const RDStructField ELF_EHDR_64LE[] = {
    {"u8[16]", "e_ident"},  {"u16", "e_type"},     {"u16", "e_machine"},
    {"u32", "e_version"},   {"u64", "e_entry"},    {"u64", "e_phoff"},
    {"u64", "e_shoff"},     {"u32", "e_flags"},    {"u16", "e_ehsize"},
    {"u16", "e_phentsize"}, {"u16", "e_phnum"},    {"u16", "e_shentsize"},
    {"u16", "e_shnum"},     {"u16", "e_shstrndx"}, {nullptr, nullptr},
};

const RDStructField ELF_EHDR_32BE[] = {
    {"u8[16]", "e_ident"},    {"u16be", "e_type"},     {"u16be", "e_machine"},
    {"u32be", "e_version"},   {"u32be", "e_entry"},    {"u32be", "e_phoff"},
    {"u32be", "e_shoff"},     {"u32be", "e_flags"},    {"u16be", "e_ehsize"},
    {"u16be", "e_phentsize"}, {"u16be", "e_phnum"},    {"u16be", "e_shentsize"},
    {"u16be", "e_shnum"},     {"u16be", "e_shstrndx"}, {nullptr, nullptr},
};

const RDStructField ELF_EHDR_64BE[] = {
    {"u8[16]", "e_ident"},    {"u16be", "e_type"},     {"u16be", "e_machine"},
    {"u32be", "e_version"},   {"u64be", "e_entry"},    {"u64be", "e_phoff"},
    {"u64be", "e_shoff"},     {"u32be", "e_flags"},    {"u16be", "e_ehsize"},
    {"u16be", "e_phentsize"}, {"u16be", "e_phnum"},    {"u16be", "e_shentsize"},
    {"u16be", "e_shnum"},     {"u16be", "e_shstrndx"}, {nullptr, nullptr},
};

const RDStructField ELF_SHDR_32LE[] = {
    {"u32", "sh_name"},    {"u32", "sh_type"},   {"u32", "sh_flags"},
    {"u32", "sh_addr"},    {"u32", "sh_offset"}, {"u32", "sh_size"},
    {"u32", "sh_link"},    {"u32", "sh_info"},   {"u32", "sh_addralign"},
    {"u32", "sh_entsize"}, {nullptr, nullptr},
};

const RDStructField ELF_SHDR_64LE[] = {
    {"u32", "sh_name"},    {"u32", "sh_type"},   {"u64", "sh_flags"},
    {"u64", "sh_addr"},    {"u64", "sh_offset"}, {"u64", "sh_size"},
    {"u32", "sh_link"},    {"u32", "sh_info"},   {"u64", "sh_addralign"},
    {"u64", "sh_entsize"}, {nullptr, nullptr},
};

const RDStructField ELF_SHDR_32BE[] = {
    {"u32be", "sh_name"},    {"u32be", "sh_type"},   {"u32be", "sh_flags"},
    {"u32be", "sh_addr"},    {"u32be", "sh_offset"}, {"u32be", "sh_size"},
    {"u32be", "sh_link"},    {"u32be", "sh_info"},   {"u32be", "sh_addralign"},
    {"u32be", "sh_entsize"}, {nullptr, nullptr},
};

const RDStructField ELF_SHDR_64BE[] = {
    {"u32be", "sh_name"},    {"u32be", "sh_type"},   {"u64be", "sh_flags"},
    {"u64be", "sh_addr"},    {"u64be", "sh_offset"}, {"u64be", "sh_size"},
    {"u32be", "sh_link"},    {"u32be", "sh_info"},   {"u64be", "sh_addralign"},
    {"u64be", "sh_entsize"}, {nullptr, nullptr},
};

const RDStructField ELF_PHDR_32LE[] = {
    {"u32", "p_type"},  {"u32", "p_offset"}, {"u32", "p_vaddr"},
    {"u32", "p_paddr"}, {"u32", "p_filesz"}, {"u32", "p_memsz"},
    {"u32", "p_flags"}, {"u32", "p_align"},  {nullptr, nullptr}};

const RDStructField ELF_PHDR_64LE[] = {
    {"u32", "p_type"},  {"u32", "p_flags"}, {"u32", "p_offset"},
    {"u64", "p_vaddr"}, {"u64", "p_paddr"}, {"u64", "p_filesz"},
    {"u64", "p_memsz"}, {"u64", "p_align"}, {nullptr, nullptr}};

const RDStructField ELF_PHDR_32BE[] = {
    {"u32be", "p_type"},  {"u32be", "p_offset"}, {"u32be", "p_vaddr"},
    {"u32be", "p_paddr"}, {"u32be", "p_filesz"}, {"u32be", "p_memsz"},
    {"u32be", "p_flags"}, {"u32be", "p_align"},  {nullptr, nullptr}};

const RDStructField ELF_PHDR_64BE[] = {
    {"u32be", "p_type"},  {"u32be", "p_flags"}, {"u32be", "p_offset"},
    {"u64be", "p_vaddr"}, {"u64be", "p_paddr"}, {"u64be", "p_filesz"},
    {"u64be", "p_memsz"}, {"u64be", "p_align"}, {nullptr, nullptr}};

} // namespace

void register_all(const ElfState& s) {
    if(s.byteorder() == BO_BIG) {
        rd_createstruct("ELF_PHDR",
                        s.bits() == 64 ? ELF_PHDR_64BE : ELF_PHDR_32BE);
    }
    else {
        rd_createstruct("ELF_PHDR",
                        s.bits() == 64 ? ELF_PHDR_64LE : ELF_PHDR_32LE);
    }
}

} // namespace elf_types
