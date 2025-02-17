#include "pluginmanager.h"
#include "../builtins/analyzer.h"
#include "../builtins/loader.h"
#include "../builtins/processor.h"
#include "../state.h"
#include "modulemanager.h"
#include <redasm/redasm.h>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>

namespace redasm::pm {

namespace {

std::unordered_map<const void*, std::string> origins;

template<typename T>
void unregister_plugins(Vect(const T*) plugins) {
    vect_foreach(const T*, plugin, plugins) {
        if((*plugin)->on_shutdown) (*plugin)->on_shutdown(*plugin);
    }
}

template<typename T>
bool register_plugin(Vect(const T*) plugins, const T* plugin,
                     const char* origin) {
    if(!plugin) return false;

    if(!origin) {
        state::error("Invalid plugin origin");
        return false;
    }

    if(!plugin->id || !(*plugin->id)) {
        state::error("Invalid plugin id");
        return false;
    }

    if(plugin->level != REDASM_API_LEVEL) {
        state::error(
            fmt::format("{}: Unsupported plugin API Level, expected {}, got {}",
                        plugin->id, REDASM_API_LEVEL, plugin->level));
        return false;
    }

    if(!plugin->name || !(*plugin->name)) {
        state::error(fmt::format("{}: Invalid plugin name", plugin->id));
        return false;
    }

    auto it =
        std::ranges::find_if(vect_begin(plugins), vect_end(plugins),
                             [&](const T* x) { return x->id == plugin->id; });

    if(it != vect_end(plugins)) {
        state::error(fmt::format("Analyzer '{}' already exists", plugin->id));
        return false;
    }

    spdlog::info(R"(Registered plugin "{}" ({}))", plugin->name, plugin->id);
    if(plugin->on_init) plugin->on_init(plugin);
    vect_add(const T*, plugins, plugin);
    pm::origins[plugin] = origin;
    return true;
}

template<typename T>
const T* find_plugin(Vect(const T*) plugins, std::string_view id) {
    vect_foreach(const T*, plugin, plugins) {
        if((*plugin)->id == id) return *plugin;
    }

    return nullptr;
}

} // namespace

void create() {
    spdlog::info("Loading Plugins");
    pm::loaders = vect_create(const RDLoaderPlugin*);
    pm::processors = vect_create(const RDProcessorPlugin*);
    pm::analyzers = vect_create(const RDAnalyzerPlugin*);
    builtins::register_loaders();
    builtins::register_processors();
    builtins::register_analyzers();
    mm::load_all();
}

void destroy() {
    spdlog::info("Unloading Plugins");
    pm::unregister_plugins(pm::analyzers);
    pm::unregister_plugins(pm::processors);
    pm::unregister_plugins(pm::loaders);
    pm::origins.clear();
    vect_destroy(pm::analyzers);
    vect_destroy(pm::processors);
    vect_destroy(pm::loaders);
    mm::unload_all();
}

const char* get_origin(const void* plugin) {
    auto it = pm::origins.find(plugin);
    if(it != pm::origins.end()) return it->second.c_str();
    return nullptr;
}

bool is_origin(const void* plugin, const char* origin) {
    const char* o = pm::get_origin(plugin);
    return o && origin && o == std::string_view{origin};
}

bool register_loader(const RDLoaderPlugin* plugin, const char* origin) {
    return pm::register_plugin(pm::loaders, plugin, origin);
}

bool register_processor(const RDProcessorPlugin* plugin, const char* origin) {
    if(plugin) {
        if(!plugin->address_size) {
            state::error(fmt::format(
                "register_processor: invalid address-size for processor '{}'",
                plugin->id));

            return false;
        }

        if(!plugin->integer_size) {
            state::error(fmt::format(
                "register_processor: invalid integer-size for processor '{}'",
                plugin->id));

            return false;
        }
    }

    return pm::register_plugin(pm::processors, plugin, origin);
}

bool register_analyzer(const RDAnalyzerPlugin* plugin, const char* origin) {
    return pm::register_plugin(pm::analyzers, plugin, origin);
}

const RDLoaderPlugin* find_loader(std::string_view id) {
    return pm::find_plugin(pm::loaders, id);
}

const RDProcessorPlugin* find_processor(std::string_view id) {
    return pm::find_plugin(pm::processors, id);
}

const RDAnalyzerPlugin* find_analyzer(std::string_view id) {
    return pm::find_plugin(pm::analyzers, id);
}

} // namespace redasm::pm
