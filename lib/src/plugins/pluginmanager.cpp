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

template<typename PluginType, typename SliceType>
void unregister_plugins(const SliceType& plugins) {
    const PluginType** p;
    slice_foreach(p, &plugins) {
        if((*p)->on_shutdown) (*p)->on_shutdown(*p);
    }
}

template<typename PluginType, typename SliceType>
const PluginType* find_plugin(const SliceType& plugins, std::string_view id) {
    const PluginType** p;
    slice_foreach(p, &plugins) {
        if((*p)->id == id) return *p;
    }
    return nullptr;
}

template<typename SliceType, typename PluginType>
bool register_plugin(SliceType& plugins, const PluginType* plugin,
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

    const PluginType** it;
    slice_foreach(it, &plugins) {
        if((*it)->id == plugin->id) {
            state::error(
                fmt::format("Analyzer '{}' already exists", plugin->id));
            return false;
        }
    }

    spdlog::info(R"(Registered plugin "{}" ({}))", plugin->name, plugin->id);
    if(plugin->on_init) plugin->on_init(plugin);
    slice_push(&plugins, plugin);
    pm::origins[plugin] = origin;
    return true;
}

} // namespace

void create() {
    spdlog::info("Loading Plugins");
    slice_init(&pm::loaders, nullptr, nullptr);
    slice_init(&pm::processors, nullptr, nullptr);
    slice_init(&pm::analyzers, nullptr, nullptr);
    builtins::register_loaders();
    builtins::register_processors();
    builtins::register_analyzers();
    mm::load_all();
}

void destroy() {
    spdlog::info("Unloading Plugins");
    pm::unregister_plugins<RDAnalyzerPlugin>(pm::analyzers);
    pm::unregister_plugins<RDProcessorPlugin>(pm::processors);
    pm::unregister_plugins<RDLoaderPlugin>(pm::loaders);
    pm::origins.clear();
    slice_destroy(&pm::analyzers);
    slice_destroy(&pm::processors);
    slice_destroy(&pm::loaders);
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
    return pm::find_plugin<RDLoaderPlugin>(pm::loaders, id);
}

const RDProcessorPlugin* find_processor(std::string_view id) {
    return pm::find_plugin<RDProcessorPlugin>(pm::processors, id);
}

const RDAnalyzerPlugin* find_analyzer(std::string_view id) {
    return pm::find_plugin<RDAnalyzerPlugin>(pm::analyzers, id);
}

} // namespace redasm::pm
