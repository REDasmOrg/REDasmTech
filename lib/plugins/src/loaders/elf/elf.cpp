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
bool accept(const RDLoaderPlugin*, const RDLoaderRequest* req) {
    ElfIdent ident;
    usize n = rdbuffer_read(req->file, 0, &ident, sizeof(ElfIdent));

    return n == sizeof(ElfIdent) && ident.ei_magic[0] == ELFMAG0 &&
           ident.ei_magic[1] == ELFMAG1 && ident.ei_magic[2] == ELFMAG2 &&
           ident.ei_magic[3] == ELFMAG3 && ident.ei_version == EV_CURRENT;

    return elf_format::get_bits(ident) == Bits;
}

template<int Bits>
bool load(RDLoader* self, RDBuffer* file) {
    using ELF = ElfFormat<Bits>;
    auto* elf = reinterpret_cast<ELF*>(self);
    elf_types::register_all(elf->ehdr.e_ident);

    if(!rdbuffer_read(file, 0, &elf->ehdr, sizeof(typename ELF::EHDR)) ||
       (elf->ehdr.e_shstrndx && elf->ehdr.e_shstrndx >= elf->ehdr.e_shnum))
        return false;

    elf->phdr.resize(elf->ehdr.e_phnum);
    elf->shdr.resize(elf->ehdr.e_shnum);

    if(!rdbuffer_read(file, elf->ehdr.e_phoff, elf->phdr.data(),
                      sizeof(typename ELF::PHDR) * elf->phdr.size()) ||
       !rdbuffer_read(file, elf->ehdr.e_shoff, elf->shdr.data(),
                      sizeof(typename ELF::SHDR) * elf->shdr.size()))
        return false;

    usize phdridx = elf->map_memory();

    // Map sections
    for(usize i = 0; i < elf->shdr.size(); i++) {
        const auto& seg = elf->shdr[i];
        RDOffset offset = seg.sh_offset, offsize = seg.sh_size;
        usize type = SEG_UNKNOWN;

        std::string name = elf->get_str(seg.sh_name, elf->ehdr.e_shstrndx,
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

    rd_setentry(elf->ehdr.e_entry, "ELF_EntryPoint");

    // Process sections (separate steps in order to fix out of order links)
    for(usize i = 0; i < elf->shdr.size(); i++) {
        const auto& seg = elf->shdr[i];

        if(seg.sh_flags & SHF_ALLOC)
            elf->parse_section_address(i);
        else
            elf->parse_section_offset(i);
    }

    if(phdridx < elf->phdr.size()) { // Map ELF_ProgramHeader
        RDType t;
        rdtype_create_n("ELF_PHDR", elf->phdr.size(), &t);
        rd_maptype(elf->ehdr.e_phoff, elf->phdr[phdridx].p_vaddr, &t);
    }

    return true;
}

template<int Bits>
const char* get_processor(RDLoader* l) {
    using ELF = ElfFormat<Bits>;
    const auto* self = reinterpret_cast<const ELF*>(l);
    bool isbe = elf_format::is_be(self->ehdr.e_ident);

    switch(self->ehdr.e_machine) {
        case EM_AVR: return "avr8";
        case EM_386: return "x86_32";
        case EM_X86_64: return "x86_64";
        case EM_XTENSA: return isbe ? "xtensabe" : "extensale";
        case EM_ARM: return isbe ? "arm32be" : "arm32le";
        case EM_AARCH64: return isbe ? "arm64be" : "arm64le";

        case EM_MIPS: {
            if(self->ehdr.e_flags & EF_MIPS_ABI_EABI64)
                return isbe ? "mips64be" : "mips64le";
            return isbe ? "mips32be" : "mips32le";
        }

        default: break;
    }

    return nullptr;
}

template<int Bits>
RDLoaderPlugin define_loader(const char* id, const char* name) {
    return {
        .level = REDASM_API_LEVEL,
        .id = id,
        .name = name,
        .create = [](const RDLoaderPlugin*) -> RDLoader* {
            return reinterpret_cast<RDLoader*>(new ElfFormat<Bits>());
        },
        .destroy =
            [](RDLoader* self) {
                delete reinterpret_cast<ElfFormat<Bits>*>(self);
            },
        .accept = accept<Bits>,
        .load = load<Bits>,
        .get_processor = get_processor<Bits>,
    };
}

RDLoaderPlugin elf32 = define_loader<32>("elf32", "ELF Executable (32 bit)");
RDLoaderPlugin elf64 = define_loader<64>("elf64", "ELF Executable (64 bit)");

} // namespace

void rdplugin_create() {
    rd_registerloader(&elf32);
    rd_registerloader(&elf64);
}
