#include <redasm/redasm.h>

namespace {

bool init(RDLoader*) { return false; }

} // namespace

void rdplugin_init() {
    RDLoader elf{};

    elf.id = "elf";
    elf.name = "ELF Executable";
    elf.init = init;
    rd_registerloader(&elf);
}
