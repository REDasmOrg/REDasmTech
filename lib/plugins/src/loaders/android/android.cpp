#include "dalvik.h"
#include "dex.h"

void rdplugin_create() {
    rd_registerloader(&dex::loader);
    rd_registerprocessor(&dalvik::processor);
}
