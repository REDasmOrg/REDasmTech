#include "dex.h"

void rdplugin_init() {
    RDLoader ldex{};
    ldex.id = "dex";
    ldex.name = "Dalvik Executable";
    ldex.init = dex::init;
    rd_registerloader(&ldex);
}
