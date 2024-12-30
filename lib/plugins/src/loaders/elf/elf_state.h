#pragma once

#include "elf_common.h"
#include "elf_header.h"
#include <string>
#include <string_view>

struct ElfState {
    RDBuffer* file;
    ElfIdent ident;
    RDOffset stroff;

    [[nodiscard]] int byteorder() const {
        return ident.ei_data == ELFDATA2MSB ? BO_BIG : BO_LITTLE;
    }

    [[nodiscard]] int bits() const {
        return ident.ei_class == ELFCLASS64 ? 64 : 32;
    }
};

namespace elf_state {

bool validate(ElfState& pr);
std::string get_string(const ElfState& pr, int idx, const std::string& fb = {});
std::string_view get_stringv(const ElfState& pr, int idx);

} // namespace elf_state
