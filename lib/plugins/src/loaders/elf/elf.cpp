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
bool parse(RDLoader* self, const RDLoaderRequest* req) {
    using ELF = ElfFormat<Bits>;
    auto* elf = reinterpret_cast<ELF*>(self);

    if(rdbuffer_read(req->file, 0, &elf->ehdr, sizeof(typename ELF::EHDR)) !=
       sizeof(typename ELF::EHDR))
        return false;

    if(elf->ehdr.e_ident.ei_magic[0] != ELFMAG0 ||
       elf->ehdr.e_ident.ei_magic[1] != ELFMAG1 ||
       elf->ehdr.e_ident.ei_magic[2] != ELFMAG2 ||
       elf->ehdr.e_ident.ei_magic[3] != ELFMAG3 ||
       elf->ehdr.e_ident.ei_version != EV_CURRENT)
        return false;

    if(elf_format::get_bits(elf->ehdr.e_ident) != Bits) return false;

    if(elf->ehdr.e_shstrndx && elf->ehdr.e_shstrndx >= elf->ehdr.e_shnum)
        return false;

    return true;
}

template<int Bits>
void load_section_header(const ElfFormat<Bits>* elf) {
    for(usize i = 0; i < elf->shdr.size(); i++) {
        const auto& seg = elf->shdr[i];
        if(!seg.sh_addr || !seg.sh_size) continue;

        RDOffset offset = seg.sh_offset, offsize = seg.sh_size;
        usize perm = 0;

        std::string name = elf->get_str(seg.sh_name, elf->ehdr.e_shstrndx,
                                        "seg_" + std::to_string(i));

        switch(seg.sh_type) {
            case SHT_PROGBITS:
                perm = seg.sh_flags & SHF_EXECINSTR ? SP_RWX : SP_RW;
                break;

            case SHT_NOBITS: offset = offsize = 0; [[fallthrough]];
            default: perm = SP_RW; break;
        }

        rd_addsegment_n(name.c_str(), seg.sh_addr, seg.sh_size, perm, Bits);

        if(offset && offsize) rd_mapfile_n(offset, seg.sh_addr, seg.sh_size);
    }

    // Process sections (separate steps in order to fix out of order links)
    for(usize i = 0; i < elf->shdr.size(); i++) {
        const auto& seg = elf->shdr[i];

        if(seg.sh_flags & SHF_ALLOC)
            elf->parse_section_address(i);
        else
            elf->parse_section_offset(i);
    }
}

template<int Bits>
void load_program_header(const ElfFormat<Bits>* elf) {
    for(const typename ElfFormat<Bits>::PHDR& phdr : elf->phdr) {
        const char* name = nullptr;

        switch(phdr.p_type) {
            case PT_LOAD: name = "LOAD"; break;
            case PT_DYNAMIC: name = "DYNAMIC"; break;
            case PT_INTERP: name = "INTERP"; break;
            case PT_NOTE: name = "NOTE"; break;
            case PT_PHDR: name = "PHDR"; break;
            default: continue;
        }

        usize perm = 0;
        if(phdr.p_flags & PF_R) perm |= SP_R;
        if(phdr.p_flags & PF_W) perm |= SP_W;
        if(phdr.p_flags & PF_X) perm |= SP_X;

        rd_addsegment_n(name, phdr.p_vaddr, phdr.p_memsz, perm, Bits);

        if(phdr.p_offset && phdr.p_filesz)
            rd_mapfile_n(phdr.p_offset, phdr.p_vaddr, phdr.p_filesz);
    }
}

template<int Bits>
bool load(RDLoader* self, RDBuffer* file) {
    using ELF = ElfFormat<Bits>;
    auto* elf = reinterpret_cast<ELF*>(self);
    elf_types::register_all(elf->ehdr.e_ident);

    elf->shdr.resize(elf->ehdr.e_shnum);
    elf->phdr.resize(elf->ehdr.e_phnum);

    rdbuffer_read(file, elf->ehdr.e_shoff, elf->shdr.data(),
                  sizeof(typename ELF::SHDR) * elf->shdr.size());
    rdbuffer_read(file, elf->ehdr.e_phoff, elf->phdr.data(),
                  sizeof(typename ELF::PHDR) * elf->phdr.size());

    if(!elf->shdr.empty())
        load_section_header(elf);
    else if(!elf->phdr.empty())
        load_program_header(elf);
    else
        return false;

    rd_setentry(elf->ehdr.e_entry, "ELF_EntryPoint");
    return true;
}

template<int Bits>
const char* get_processor(RDLoader* self) {
    using ELF = ElfFormat<Bits>;
    const auto* elf = reinterpret_cast<const ELF*>(self);
    bool isbe = elf_format::is_be(elf->ehdr.e_ident);

    switch(elf->ehdr.e_machine) {
        case EM_AVR: return "avr8";
        case EM_386: return "x86_32";
        case EM_X86_64: return "x86_64";
        case EM_XTENSA: return isbe ? "xtensabe" : "extensale";
        case EM_ARM: return isbe ? "arm32be" : "arm32le";
        case EM_AARCH64: return isbe ? "arm64be" : "arm64le";

        case EM_MIPS: {
            if(elf->ehdr.e_flags & EF_MIPS_ABI_EABI64)
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
        .parse = parse<Bits>,
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
