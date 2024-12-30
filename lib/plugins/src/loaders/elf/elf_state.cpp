#include "elf_state.h"
#include <redasm/redasm.h>

namespace elf_state {

bool validate(ElfState& pr) {
    pr.file = rdbuffer_getfile();
    rdbuffer_read(pr.file, 0, &pr.ident, sizeof(ElfIdent));

    return pr.ident.ei_magic[0] == ELFMAG0 && pr.ident.ei_magic[1] == ELFMAG1 &&
           pr.ident.ei_magic[2] == ELFMAG2 && pr.ident.ei_magic[3] == ELFMAG3 &&
           pr.ident.ei_version == EV_CURRENT;
}

std::string get_string(const ElfState& pr, int idx, const std::string& fb) {
    const char* p = nullptr;
    if(rdbuffer_getstrz(pr.file, pr.stroff + idx, &p))
        return p;
    return fb;
}

std::string_view get_stringv(const ElfState& pr, int idx) {
    const char* p = nullptr;
    if(rdbuffer_getstrz(pr.file, pr.stroff + idx, &p))
        return p;
    return {};
}

} // namespace elf_state
