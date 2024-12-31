#include "elf_state.h"
#include <redasm/redasm.h>

namespace elf_state {

void set_processor(const ElfState& s, u16 machine, u32 flags) {
    switch(machine) {
        case EM_AVR: rd_setprocessor("avr8"); break;
        case EM_386: rd_setprocessor("x86_32"); break;
        case EM_X86_64: rd_setprocessor("x86_64"); break;

        case EM_XTENSA:
            rd_setprocessor(s.is_be() ? "xtensabe" : "extensale");
            break;

        case EM_AARCH64:
            rd_setprocessor(s.is_be() ? "arm64be" : "arm64le");
            break;

        case EM_MIPS: {
            if(flags & EF_MIPS_ABI_EABI64)
                rd_setprocessor(s.is_be() ? "mips64be" : "mips64le");
            else
                rd_setprocessor(s.is_be() ? "mips32be" : "mips32le");
            break;
        }

        default: break;
    }
}

bool validate(ElfState& s) {
    s.file = rdbuffer_getfile();
    rdbuffer_read(s.file, 0, &s.ident, sizeof(ElfIdent));

    return s.ident.ei_magic[0] == ELFMAG0 && s.ident.ei_magic[1] == ELFMAG1 &&
           s.ident.ei_magic[2] == ELFMAG2 && s.ident.ei_magic[3] == ELFMAG3 &&
           s.ident.ei_version == EV_CURRENT;
}

std::string get_string(const ElfState& s, int idx, usize shidx,
                       const std::string& fb) {
    const char* p = nullptr;
    if(rdbuffer_getstrz(s.file, s.shstrings.at(shidx) + idx, &p))
        return p;
    return fb;
}

std::string_view get_stringv(const ElfState& s, int idx, usize shidx) {
    const char* p = nullptr;
    if(rdbuffer_getstrz(s.file, s.shstrings.at(shidx) + idx, &p))
        return p;
    return {};
}

} // namespace elf_state
