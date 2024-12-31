#pragma once

#include "elf_state.h"
#include <redasm/redasm.h>
#include <string_view>

namespace elf_sections {

void parse(const ElfState& s, std::string_view name, RDAddress address,
           u64 size);

}
