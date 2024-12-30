#include "elf_sections.h"

namespace elf_sections {

void parse(std::string_view name, RDAddress address) {
    if(name == ".interp")
        rd_settypename(address, "str");
}

} // namespace elf_sections
