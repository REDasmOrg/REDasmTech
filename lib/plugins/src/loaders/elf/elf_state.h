#pragma once

#include "elf_common.h"
#include "elf_header.h"
#include <string>
#include <string_view>

struct ElfState {
    RDBuffer* file;
    ElfIdent ident;
    std::unordered_map<usize, RDOffset> shstrings;

    [[nodiscard]] bool is_be() const { return ident.ei_data == ELFDATA2MSB; }

    [[nodiscard]] int bits() const {
        return ident.ei_class == ELFCLASS64 ? 64 : 32;
    }
};

namespace elf_state {

bool validate(ElfState& s);
void set_processor(const ElfState& s, u16 machine, u32 flags);
std::string get_string(const ElfState& s, int idx, usize shidx = 0,
                       const std::string& fb = {});
std::string_view get_stringv(const ElfState& s, int idx, usize shidx = 0);

} // namespace elf_state
