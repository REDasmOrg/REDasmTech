#include "elf_types.h"
#include "elf_format.h"

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

// const RDStructField ELF_SHDR_32LE[] = {
//     {"u32", "sh_name"},    {"u32", "sh_type"},   {"u32", "sh_flags"},
//     {"u32", "sh_addr"},    {"u32", "sh_offset"}, {"u32", "sh_size"},
//     {"u32", "sh_link"},    {"u32", "sh_info"},   {"u32", "sh_addralign"},
//     {"u32", "sh_entsize"}, {nullptr, nullptr},
// };

// const RDStructField ELF_SHDR_64LE[] = {
//     {"u32", "sh_name"},    {"u32", "sh_type"},   {"u64", "sh_flags"},
//     {"u64", "sh_addr"},    {"u64", "sh_offset"}, {"u64", "sh_size"},
//     {"u32", "sh_link"},    {"u32", "sh_info"},   {"u64", "sh_addralign"},
//     {"u64", "sh_entsize"}, {nullptr, nullptr},
// };

// const RDStructField ELF_SHDR_32BE[] = {
//     {"u32be", "sh_name"},    {"u32be", "sh_type"},   {"u32be", "sh_flags"},
//     {"u32be", "sh_addr"},    {"u32be", "sh_offset"}, {"u32be", "sh_size"},
//     {"u32be", "sh_link"},    {"u32be", "sh_info"},   {"u32be",
//     "sh_addralign"},
//     {"u32be", "sh_entsize"}, {nullptr, nullptr},
// };

// const RDStructField ELF_SHDR_64BE[] = {
//     {"u32be", "sh_name"},    {"u32be", "sh_type"},   {"u64be", "sh_flags"},
//     {"u64be", "sh_addr"},    {"u64be", "sh_offset"}, {"u64be", "sh_size"},
//     {"u32be", "sh_link"},    {"u32be", "sh_info"},   {"u64be",
//     "sh_addralign"},
//     {"u64be", "sh_entsize"}, {nullptr, nullptr},
// };

const RDStructField ELF_DYN_32LE[] = {
    {"i32", "d_tag"}, {"u32", "d_val"}, {nullptr, nullptr}};

const RDStructField ELF_DYN_64LE[] = {
    {"i64", "d_tag"}, {"u64", "d_val"}, {nullptr, nullptr}};

const RDStructField ELF_DYN_32BE[] = {
    {"i32be", "d_tag"}, {"u32be", "d_val"}, {nullptr, nullptr}};

const RDStructField ELF_DYN_64BE[] = {
    {"i64be", "d_tag"}, {"u64be", "d_val"}, {nullptr, nullptr}};

const RDStructField ELF_SYM_32LE[] = {
    {"u32", "st_name"}, {"u32", "st_value"}, {"u32", "st_size"},
    {"u8", "st_info"},  {"u8", "st_other"},  {"u16", "st_shndx"},
    {nullptr, nullptr},
};

const RDStructField ELF_SYM_32BE[] = {
    {"u32be", "st_name"}, {"u32be", "st_value"}, {"u32be", "st_size"},
    {"u8", "st_info"},    {"u8", "st_other"},    {"u16be", "st_shndx"},
    {nullptr, nullptr},
};

const RDStructField ELF_SYM_64LE[] = {
    {"u32", "st_name"},  {"u8", "st_info"},   {"u8", "st_other"},
    {"u16", "st_shndx"}, {"u64", "st_value"}, {"u64", "st_size"},
    {nullptr, nullptr},
};

const RDStructField ELF_SYM_64BE[] = {
    {"u32be", "st_name"},  {"u8", "st_info"},     {"u8", "st_other"},
    {"u16be", "st_shndx"}, {"u64be", "st_value"}, {"u64be", "st_size"},
    {nullptr, nullptr},
};

const RDStructField ELF_REL_32LE[] = {
    {"u32", "r_offset"}, {"u32", "r_info"}, {nullptr, nullptr}};

const RDStructField ELF_REL_64LE[] = {
    {"u64", "r_offset"}, {"u64", "r_info"}, {nullptr, nullptr}};

const RDStructField ELF_REL_32BE[] = {
    {"u32be", "r_offset"}, {"u32be", "r_info"}, {nullptr, nullptr}};

const RDStructField ELF_REL_64BE[] = {
    {"u64be", "r_offset"}, {"u64be", "r_info"}, {nullptr, nullptr}};

const RDStructField ELF_RELA_32LE[] = {{"u32", "r_offset"},
                                       {"u32", "r_info"},
                                       {"i32", "r_addend"},
                                       {nullptr, nullptr}};

const RDStructField ELF_RELA_64LE[] = {{"u64", "r_offset"},
                                       {"u64", "r_info"},
                                       {"i64", "r_addend"},
                                       {nullptr, nullptr}};

const RDStructField ELF_RELA_32BE[] = {{"u32be", "r_offset"},
                                       {"u32be", "r_info"},
                                       {"i32be", "r_addend"},
                                       {nullptr, nullptr}};

const RDStructField ELF_RELA_64BE[] = {{"u64be", "r_offset"},
                                       {"u64be", "r_info"},
                                       {"i64be", "r_addend"},
                                       {nullptr, nullptr}};

const RDStructField ELF_VERNEED_LE[] = {
    {"u16", "vn_version"}, {"u16", "vn_cnt"},  {"u32", "vn_file"},
    {"u32", "vn_aux"},     {"u32", "vn_next"}, {nullptr, nullptr}};

const RDStructField ELF_VERNEED_BE[] = {
    {"u16be", "vn_version"}, {"u16be", "vn_cnt"},  {"u32be", "vn_file"},
    {"u32be", "vn_aux"},     {"u32be", "vn_next"}, {nullptr, nullptr}};

} // namespace

void register_all(const ElfIdent& ident) {
    bool isbe = elf_format::is_be(ident);
    int b = elf_format::get_bits(ident);

    if(isbe) {
        rd_createstruct("ELF_VERNEED", ELF_VERNEED_BE);
        rd_createstruct("ELF_PHDR", b == 64 ? ELF_PHDR_64BE : ELF_PHDR_32BE);
        // rd_createstruct("ELF_SHDR", b == 64 ? ELF_SHDR_64BE : ELF_SHDR_32BE);
        rd_createstruct("ELF_DYN", b == 64 ? ELF_DYN_64BE : ELF_DYN_32BE);
        rd_createstruct("ELF_SYM", b == 64 ? ELF_SYM_64BE : ELF_SYM_32BE);
        rd_createstruct("ELF_REL", b == 64 ? ELF_REL_64BE : ELF_REL_32BE);
        rd_createstruct("ELF_RELA", b == 64 ? ELF_RELA_64BE : ELF_RELA_32BE);
    }
    else {
        rd_createstruct("ELF_VERNEED", ELF_VERNEED_LE);
        rd_createstruct("ELF_PHDR", b == 64 ? ELF_PHDR_64LE : ELF_PHDR_32LE);
        // rd_createstruct("ELF_SHDR", b == 64 ? ELF_SHDR_64LE : ELF_SHDR_32LE);
        rd_createstruct("ELF_DYN", b == 64 ? ELF_DYN_64LE : ELF_DYN_32LE);
        rd_createstruct("ELF_SYM", b == 64 ? ELF_SYM_64LE : ELF_SYM_32LE);
        rd_createstruct("ELF_REL", b == 64 ? ELF_REL_64LE : ELF_REL_32LE);
        rd_createstruct("ELF_RELA", b == 64 ? ELF_RELA_64LE : ELF_RELA_32LE);
    }
}

} // namespace elf_types
