#include "elf_types.h"

namespace elf_types {

namespace {

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
    if(s.is_be() == BO_BIG) {
        rd_createstruct("ELF_PHDR",
                        s.bits() == 64 ? ELF_PHDR_64BE : ELF_PHDR_32BE);
    }
    else {
        rd_createstruct("ELF_PHDR",
                        s.bits() == 64 ? ELF_PHDR_64LE : ELF_PHDR_32LE);
    }
}

} // namespace elf_types
