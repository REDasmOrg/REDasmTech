#include "../../plugins/pluginmanager.h"
#include <redasm/plugin.h>

const char* rdplugin_getorigin(const void* plugin) {
    return redasm::pm::get_origin(plugin);
}
