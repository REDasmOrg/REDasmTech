#include "../internal/plugin.h"
#include <redasm/plugin.h>

bool rd_registerplugin(const RDPluginDefinition* plugin) {
    return redasm::api::internal::register_plugin(plugin);
}

const RDPluginDefinition** rd_getplugins(int type, usize* n) {
    return redasm::api::internal::get_plugins(type, n);
}
