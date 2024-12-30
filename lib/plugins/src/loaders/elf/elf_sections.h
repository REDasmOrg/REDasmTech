#pragma once

#include <redasm/redasm.h>
#include <string_view>

namespace elf_sections {

void parse(std::string_view name, RDAddress address);

}
