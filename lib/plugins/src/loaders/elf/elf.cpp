#include "elf_common.h"
#include "elf_header.h"
#include "elf_sections.h"
#include "elf_state.h"
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

template<int bits>
void map_memory(const std::vector<ElfPhdr<bits>>& phdr, usize& phdridx) {
    RDAddress start = -1ULL, end = 0;

    for(usize i = 0; i < phdr.size(); i++) {
        const auto& p = phdr[i];

        switch(p.p_type) {
            case PT_LOAD: {
                start = std::min<RDAddress>(start, p.p_vaddr);

                u64 asize = p.p_memsz; // Align size
                if(usize diff = asize % p.p_align; diff)
                    asize += (p.p_align - diff);

                RDAddress pend = p.p_vaddr + asize;
                end = std::max<RDAddress>(end, pend);
                break;
            }

            case PT_PHDR: phdridx = i; break;
            default: break;
        }
    }

    rd_map(start, end);
}

template<int bits>
bool load_elf(ElfState& pr) {
    ElfEhdr<bits> ehdr;
    if(!rdbuffer_read(pr.file, 0, &ehdr, sizeof(ehdr)) ||
       ehdr.e_shstrndx >= ehdr.e_shnum)
        return false;

    std::vector<ElfPhdr<bits>> phdr(ehdr.e_phnum);
    if(!rdbuffer_read(pr.file, ehdr.e_phoff, phdr.data(),
                      sizeof(ElfPhdr<bits>) * phdr.size()))
        return false;

    std::vector<ElfShdr<bits>> shdr(ehdr.e_shnum);
    if(!rdbuffer_read(pr.file, ehdr.e_shoff, shdr.data(),
                      sizeof(ElfShdr<bits>) * shdr.size()))
        return false;

    pr.stroff = shdr[ehdr.e_shstrndx].sh_offset;

    usize phdridx = phdr.size();
    map_memory<bits>(phdr, phdridx);

    for(usize i = 0; i < shdr.size(); i++) {
        const auto& s = shdr[i];

        if(!(s.sh_flags & SHF_ALLOC))
            continue;

        usize type = SEG_UNKNOWN;
        RDOffset offset = s.sh_offset;
        usize size = s.sh_size;
        std::string name =
            elf_state::get_string(pr, s.sh_name, "seg_" + std::to_string(i));

        switch(s.sh_type) {
            case SHT_PROGBITS:
                type = s.sh_flags & SHF_EXECINSTR ? SEG_HASCODE : SEG_HASDATA;
                break;

            case SHT_NOBITS: offset = size = 0; break;
            default: type = SEG_HASDATA; break;
        }

        rd_mapsegment_n(name.c_str(), s.sh_addr, s.sh_size, offset, size, type);
        elf_sections::parse(name, s.sh_addr);
    }

    if(phdridx < phdr.size()) { // Map ELF_ProgramHeader
        RDType t;
        if(rd_createtype_n("ELF_PHDR", phdr.size(), &t))
            rd_maptype(ehdr.e_phoff, phdr[phdridx].p_vaddr, &t);
    }

    return true;
}

bool init(RDLoader*) {
    ElfState s{};
    if(!elf_state::validate(s))
        return false;

    elf_types::register_all(s);

    switch(s.bits()) {
        case 32: load_elf<32>(s); break;
        case 64: load_elf<64>(s); break;
        default: return false;
    }

    return true;
}

} // namespace

void rdplugin_init() {
    RDLoader elf{};
    elf.id = "elf";
    elf.name = "ELF Executable";
    elf.init = init;
    rd_registerloader(&elf);
}
