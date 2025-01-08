#include "pluginmanager.h"
#include "../builtins/analyzer.h"
#include "../builtins/loader.h"
#include "../builtins/processor.h"
#include "../state.h"
#include "modulemanager.h"
#include <spdlog/spdlog.h>
#include <vector>

namespace redasm::pm {

namespace {

std::vector<const RDLoaderPlugin*> loaders;
std::vector<const RDProcessorPlugin*> processors;
std::vector<const RDAnalyzerPlugin*> analyzers;

template<typename T>
void unregister_plugins(const std::vector<const T*>& plugins) {
    for(const T* plugin : plugins) {
        if(plugin->onshutdown) plugin->onshutdown(plugin);
    }
}

template<typename T>
bool register_plugin(std::vector<const T*>& plugins, const T* plugin) {
    if(!plugin) return false;

    if(!plugin->id || !(*plugin->id)) {
        state::error("Invalid plugin id");
        return false;
    }
    if(!plugin->name || !(*plugin->name)) {
        state::error("Invalid plugin name");
        return false;
    }

    auto it = std::ranges::find_if(
        plugins, [&](const T* x) { return x->id == plugin->id; });

    if(it != plugins.end()) {
        state::error(fmt::format("Analyzer '{}' already exists", plugin->id));
        return false;
    }

    spdlog::info(R"(Registered plugin "{}" ({}))", plugin->name, plugin->id);
    if(plugin->oninit) plugin->oninit(plugin);
    plugins.push_back(plugin);
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
    pm::analyzers.clear();
    pm::processors.clear();
    pm::loaders.clear();
    mm::unload_all();
}

bool register_loader(const RDLoaderPlugin* plugin) {
    return pm::register_plugin(pm::loaders, plugin);
}

bool register_processor(const RDProcessorPlugin* plugin) {
    return pm::register_plugin(pm::processors, plugin);
}

bool register_analyzer(const RDAnalyzerPlugin* plugin) {
    return pm::register_plugin(pm::analyzers, plugin);
}

const RDLoaderPlugin** get_loaders(usize* n) {
    if(n) *n = pm::loaders.size();
    return pm::loaders.data();
}

const RDProcessorPlugin** get_processors(usize* n) {
    if(n) *n = pm::processors.size();
    return pm::processors.data();
}

const RDAnalyzerPlugin** get_analyzers(usize* n) {
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
