#pragma once

#include "elf_common.h"
#include "elf_dwarf.h"
#include "elf_header.h"
#include <string>
#include <string_view>
#include <vector>

template<int Bits>
struct ElfFormat {
    using EHDR = ElfEhdr<Bits>;
    using PHDR = ElfPhdr<Bits>;
    using SHDR = ElfShdr<Bits>;
    using SYM = ElfSym<Bits>;
    using REL = ElfRel<Bits>;
    using RELA = ElfRela<Bits>;
    using SVAL = ElfSignedT<Bits>;
    using UVAL = ElfUnsignedT<Bits>;

    EHDR ehdr;
    std::vector<PHDR> phdr;
    std::vector<SHDR> shdr;

    [[nodiscard]] usize map_memory() const;
    [[nodiscard]] std::string get_str(int idx, usize shidx = 0,
                                      const std::string& d = {}) const;
    [[nodiscard]] std::string_view get_strv(int idx, usize shidx = 0) const;
    void parse_section_address(usize i) const;
    void parse_section_offset(usize i) const;
    void parse_dwarf_fde(RDAddress& addr, RDAddress prevaddr, u32 len,
                         const elf_dwarf::CIEInfo& cie) const;
    elf_dwarf::CIEInfo parse_dwarf_cie(RDAddress& addr, RDAddress prevaddr,
                                       u32 len) const;

private:
    void apply_type(const char* tname, const SHDR& shdr) const;
    void process_init_fini(const SHDR& shdr, const std::string& prefix) const;
    void process_strings(const SHDR& shdr) const;
    void process_symtab_offset(const SHDR& shdr) const;
    void process_symtab_address(const SHDR& shdr) const;
    void process_rela(const SHDR& shdr) const;
    void process_rel(const SHDR& shdr) const;
    void process_eh_frame_hdr(const SHDR& shdr) const;
    void process_eh_frame(const SHDR& shdr) const;
};

namespace elf_format {

inline bool is_be(const ElfIdent& ident) {
    return ident.ei_data == ELFDATA2MSB;
}

inline int get_bits(const ElfIdent& ident) {
    return ident.ei_class == ELFCLASS64 ? 64 : 32;
}

} // namespace elf_format
