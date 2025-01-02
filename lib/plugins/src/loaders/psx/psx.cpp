#include "psxexe.h"
#include <redasm/redasm.h>

void rdplugin_init() {
    RDLoader lpsxexe{};
    lpsxexe.id = "psxexe";
    lpsxexe.name = "PS-X Executable";
    lpsxexe.init = psxexe::init;
    rd_registerloader(&lpsxexe);
}
