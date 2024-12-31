#pragma once

#include "elf_state.h"
#include <redasm/redasm.h>
#include <string_view>

namespace elf_sections {

void parse_address(const ElfState& s, std::string_view name, u32 shtype,
                   RDAddress address, u64 size, u32 shlink);

void parse_offset(const ElfState& s, u32 shtype, RDOffset offset, u64 size,
                  u32 shlink);

} // namespace elf_sections
