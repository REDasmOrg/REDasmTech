#include "pluginmanager.h"
#include "../builtins/analyzer.h"
#include "../builtins/loader.h"
#include "../builtins/processor.h"
#include "../state.h"
#include "modulemanager.h"
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace redasm::pm {

namespace {

std::vector<const RDLoaderPlugin*> loaders;
std::vector<const RDProcessorPlugin*> processors;
std::vector<const RDAnalyzerPlugin*> analyzers;
std::unordered_map<const void*, std::string> origins;

template<typename T>
void unregister_plugins(const std::vector<const T*>& plugins) {
    for(const T* plugin : plugins) {
        if(plugin->on_shutdown) plugin->on_shutdown(plugin);
    }
}

template<typename T>
bool register_plugin(std::vector<const T*>& plugins, const T* plugin,
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

    auto it = std::ranges::find_if(
        plugins, [&](const T* x) { return x->id == plugin->id; });

    if(it != plugins.end()) {
        state::error(fmt::format("Analyzer '{}' already exists", plugin->id));
        return false;
    }

    spdlog::info(R"(Registered plugin "{}" ({}))", plugin->name, plugin->id);
    if(plugin->on_init) plugin->on_init(plugin);
    plugins.push_back(plugin);
    pm::origins[plugin] = origin;
    return true;
}

template<typename T>
const T* find_plugin(std::vector<const T*>& plugins, std::string_view id) {
    for(const T* plugin : plugins) {
        if(plugin->id == id) return plugin;
    }

    return nullptr;
}

} // namespace

void create() {
    spdlog::info("Loading Plugins");
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
    pm::analyzers.clear();
    pm::processors.clear();
    pm::loaders.clear();
    mm::unload_all();
}

const char* get_origin(const void* plugin) {
    auto it = pm::origins.find(plugin);
    if(it != pm::origins.end()) return it->second.c_str();
    return nullptr;
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

const RDLoaderPlugin** get_loaderplugins(usize* n) {
    if(n) *n = pm::loaders.size();
    return pm::loaders.data();
}

const RDProcessorPlugin** get_processorplugins(usize* n) {
    if(n) *n = pm::processors.size();
    return pm::processors.data();
}

const RDAnalyzerPlugin** get_analyzerplugins(usize* n) {
    if(n) *n = pm::analyzers.size();
    return pm::analyzers.data();
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
