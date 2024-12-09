#include "psxexe.h"
#include <redasm/redasm.h>

void rdplugin_init() {
    RDLoader ldrpsxexe{};
    ldrpsxexe.id = "psxexe";
    ldrpsxexe.name = "PS-X Executable";
    ldrpsxexe.init = psxexe::init;
    rd_registerloader(&ldrpsxexe);
}
