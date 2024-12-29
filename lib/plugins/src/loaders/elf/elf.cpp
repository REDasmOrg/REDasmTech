#include "elf_common.h"
#include "elf_header.h"
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

struct ValidateResult {
    RDBuffer* file;
    int byteorder;
    int bits;
};

bool validate_elf(ValidateResult& vr) {
    vr.file = rdbuffer_getfile();
    RDValue* ident = rdvalue_create();

    if(!rdbuffer_gettype(vr.file, 0, "u8[16]", ident))
        return false;

    u8 val, byteorder, bits;

    if(!rdvalue_getu8(ident, "[0]", &val) || val != ELFMAG0 ||
       !rdvalue_getu8(ident, "[1]", &val) || val != ELFMAG1 ||
       !rdvalue_getu8(ident, "[2]", &val) || val != ELFMAG2 ||
       !rdvalue_getu8(ident, "[3]", &val) || val != ELFMAG3 ||
       !rdvalue_getu8(ident, "[6]", &val) || val != EV_CURRENT ||
       !rdvalue_getu8(ident, "[4]", &bits) ||
       !rdvalue_getu8(ident, "[5]", &byteorder))
        return false;

    switch(byteorder) {
        case ELFDATA2MSB: vr.byteorder = BO_BIG; break;
        case ELFDATA2LSB: vr.byteorder = BO_LITTLE; break;
        default: return false;
    }

    switch(bits) {
        case ELFCLASS32: vr.bits = 32; break;
        case ELFCLASS64: vr.bits = 64; break;
        default: return false;
    }

    return true;
}

void register_types(const ValidateResult& vr) {
    if(vr.byteorder == BO_BIG) {
        rd_createstruct("ELF_EHDR",
                        vr.bits == 64 ? ELF_EHDR_64BE : ELF_EHDR_32BE);
        rd_createstruct("ELF_SHDR",
                        vr.bits == 64 ? ELF_SHDR_64BE : ELF_SHDR_32BE);
        rd_createstruct("ELF_PHDR",
                        vr.bits == 64 ? ELF_PHDR_64BE : ELF_PHDR_32BE);
    }
    else {
        rd_createstruct("ELF_EHDR",
                        vr.bits == 64 ? ELF_EHDR_64LE : ELF_EHDR_32LE);
        rd_createstruct("ELF_SHDR",
                        vr.bits == 64 ? ELF_SHDR_64LE : ELF_SHDR_32LE);
        rd_createstruct("ELF_PHDR",
                        vr.bits == 64 ? ELF_PHDR_64LE : ELF_PHDR_32LE);
    }
}

bool memory_map(RDBuffer* file, const RDValue* ehdr) {
    u64 phoff, phnum;
    RDValue* phdr = rdvalue_create();

    if(!rdvalue_getu64(ehdr, "e_phoff", &phoff) ||
       !rdvalue_getu64(ehdr, "e_phnum", &phnum))
        return false;

    // TODO(davide): Implement string formatting
    const std::string T = "ELF_PHDR[" + std::to_string(phnum) + "]";

    if(!rdbuffer_collecttype(file, phoff, T.c_str(), phdr))
        return false;

    u64 base = -1U, size = 0;
    const RDValue* item = rdvalue_create();

    for(u64 i = 0; i < phnum; i++) {
        if(!rdvalue_at(phdr, i, &item))
            return false;

        u64 itype, ibase, isize;

        if(!rdvalue_getu64(item, "p_type", &itype) ||
           !rdvalue_getu64(item, "p_vaddr", &ibase) ||
           !rdvalue_getu64(item, "p_memsz", &isize))
            return false;

        if(itype == PT_LOAD) {
            base = std::min<u64>(base, ibase);
            size += isize;
        }
    }

    rd_map_n(base, size);
    return true;
}

bool init(RDLoader*) {
    ValidateResult vr{};
    if(!validate_elf(vr))
        return false;

    register_types(vr);
    RDValue* ehdr = rdvalue_create();

    if(!rdbuffer_collecttype(vr.file, 0, "ELF_EHDR", ehdr))
        return false;

    memory_map(vr.file, ehdr);

    // u64 shoff, phoff;

    // if(!rdvalue_getu64(ehdr, "e_shoff", &shoff) ||
    //    !rdvalue_getu64(ehdr, "e_phoff", &phoff) ||
    //    !rdbuffer_collecttype(vr.file, shoff, "ELF_SHDR", shdr) ||
    //    !rdbuffer_collecttype(vr.file, phoff, "ELF_PHDR", phdr) ||
    //    !prepare_mapping(ehdr, phdr))
    //     return false;

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
