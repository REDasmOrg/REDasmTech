#include "psxexe.h"
#include <redasm/redasm.h>

void rdplugin_create() { rd_registerloader(&psxexe::loader); }
