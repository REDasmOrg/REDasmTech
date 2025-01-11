#include "elf_common.h"
#include "elf_format.h"
#include "elf_types.h"
#include <redasm/redasm.h>
#include <string>

// clang-format off
/*
 * ELF:
 *   - https://docs.oracle.com/cd/E19683-01/816-1386/6m7qcoblh/index.html
 *   - https://refspecs.linuxfoundation.org/elf/gabi41.pdf
 *
 * Symbol Table:
 *   - https://docs.oracle.com/cd/E19683-01/816-1386/6m7qcoblj/index.html#chapter6-79797
 *   - https://refspecs.linuxfoundation.org/elf/gabi4+/ch4.symtab.html
 *   - https://github.com/bminor/binutils-gdb/blob/master/binutils/readelf.c
 *
 * Segment Header:
 *   - https://docs.oracle.com/cd/E19683-01/816-1386/chapter6-94076/index.html
 */
// clang-format on

namespace {

template<int Bits>
bool load_elf() {
    using ELF = ElfFormat<Bits>;
    ELF elf{};
    RDBuffer* file = rdbuffer_getfile();

    if(!rdbuffer_read(file, 0, &elf.ehdr, sizeof(typename ELF::EHDR)) ||
       (elf.ehdr.e_shstrndx && elf.ehdr.e_shstrndx >= elf.ehdr.e_shnum))
        return false;

    elf.phdr.resize(elf.ehdr.e_phnum);
    elf.shdr.resize(elf.ehdr.e_shnum);

    if(!rdbuffer_read(file, elf.ehdr.e_phoff, elf.phdr.data(),
                      sizeof(typename ELF::PHDR) * elf.phdr.size()) ||
       !rdbuffer_read(file, elf.ehdr.e_shoff, elf.shdr.data(),
                      sizeof(typename ELF::SHDR) * elf.shdr.size()))
        return false;

    usize phdridx = elf.map_memory();
    elf.set_processor();

    // Map sections
    for(usize i = 0; i < elf.shdr.size(); i++) {
        const auto& seg = elf.shdr[i];
        RDOffset offset = seg.sh_offset, offsize = seg.sh_size;
        usize type = SEG_UNKNOWN;

        std::string name = elf.get_str(seg.sh_name, elf.ehdr.e_shstrndx,
                                       "seg_" + std::to_string(i));

        switch(seg.sh_type) {
            case SHT_PROGBITS:
                type = seg.sh_flags & SHF_EXECINSTR ? SEG_HASCODE : SEG_HASDATA;
                break;

            case SHT_NOBITS: offset = offsize = 0; [[fallthrough]];
            default: type = SEG_HASDATA; break;
        }

        rd_mapsegment_n(name.c_str(), seg.sh_addr, seg.sh_size, offset, offsize,
                        type);
    }

    // Process sections (separate steps in order to fix out of order links)
    for(usize i = 0; i < elf.shdr.size(); i++) {
        const auto& seg = elf.shdr[i];

        if(seg.sh_flags & SHF_ALLOC)
            elf.parse_section_address(i);
        else
            elf.parse_section_offset(i);
    }

    if(phdridx < elf.phdr.size()) { // Map ELF_ProgramHeader
        RDType t;
        rdtype_create_n("ELF_PHDR", elf.phdr.size(), &t);
        rd_maptype(elf.ehdr.e_phoff, elf.phdr[phdridx].p_vaddr, &t);
    }

    rd_setentry(elf.ehdr.e_entry, "ELF_EntryPoint");
    return true;
}

bool accept(const RDLoaderPlugin*, RDBuffer* file) {
    ElfIdent ident;
    usize n = rdbuffer_read(file, 0, &ident, sizeof(ElfIdent));

    return n == sizeof(ElfIdent) && ident.ei_magic[0] == ELFMAG0 &&
           ident.ei_magic[1] == ELFMAG1 && ident.ei_magic[2] == ELFMAG2 &&
           ident.ei_magic[3] == ELFMAG3 && ident.ei_version == EV_CURRENT;
}

bool load(RDLoader*) {
    ElfIdent ident;
    rdbuffer_read(rdbuffer_getfile(), 0, &ident, sizeof(ElfIdent));
    elf_types::register_all(ident);

    switch(elf_format::get_bits(ident)) {
        case 32: return load_elf<32>();
        case 64: return load_elf<64>();
        default: break;
    }

    return false;
}

RDLoaderPlugin loader = {
    .id = "elf",
    .name = "ELF Executable",
    .accept = accept,
    .load = load,
};

} // namespace

void rdplugin_create() { rd_registerloader(&loader); }
