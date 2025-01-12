#include "com.h"
#include "dos.h"
#include <redasm/redasm.h>

void rdplugin_create() {
    rd_registerloader(&com::loader);
    rd_registerloader(&dos::loader);
}
