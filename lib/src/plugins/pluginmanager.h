#pragma once

#include "../error.h"
#include <redasm/redasm.h>
#include <string_view>

namespace redasm::pm {

constexpr const char* NATIVE = "native";

#define _foreach_plugins(type, item, ...)                                      \
    do {                                                                       \
        usize n;                                                               \
        const auto** plugins = redasm::pm::get_##type##plugins(&n);            \
        for(usize i = 0; i < n; i++) {                                         \
            const auto* item = plugins[i];                                     \
            __VA_ARGS__                                                        \
        }                                                                      \
    } while(0);

// clang-format off
#define foreach_loaders(item, ...) _foreach_plugins(loader, item, __VA_ARGS__)
#define foreach_processors(item, ...) _foreach_plugins(processor, item, __VA_ARGS__)
#define foreach_analyzers(item, ...) _foreach_plugins(analyzer, item, __VA_ARGS__)

template<typename T> struct InstanceForPlugin {};
template<> struct InstanceForPlugin<RDLoaderPlugin> { using Type = RDLoader; };
template<> struct InstanceForPlugin<RDProcessorPlugin> { using Type = RDProcessor; };
template<> struct InstanceForPlugin<RDAnalyzerPlugin> { using Type = RDAnalyzer; };
// clang-format on

void create();
void destroy();
const char* get_origin(const void* plugin);

bool register_loader(const RDLoaderPlugin* plugin, const char* origin);
bool register_processor(const RDProcessorPlugin* plugin, const char* origin);
bool register_analyzer(const RDAnalyzerPlugin* plugin, const char* origin);

const RDLoaderPlugin** get_loaderplugins(usize* n);
const RDProcessorPlugin** get_processorplugins(usize* n);
const RDAnalyzerPlugin** get_analyzerplugins(usize* n);

const RDLoaderPlugin* find_loader(std::string_view id);
const RDProcessorPlugin* find_processor(std::string_view id);
const RDAnalyzerPlugin* find_analyzer(std::string_view id);

template<typename T>
pm::InstanceForPlugin<T>::Type* create_instance(const T* plugin) {
    assume(plugin);
    using Instance = pm::InstanceForPlugin<T>::Type;

    Instance* instance = nullptr;
    if(plugin->create)
        instance = reinterpret_cast<Instance*>(plugin->create(plugin));
    return instance;
}

template<typename T>
void destroy_instance(const T* plugin,
                      typename pm::InstanceForPlugin<T>::Type* p) {
    if(!plugin && !p) return;
    assume(plugin);
    if(plugin->destroy && p) plugin->destroy(p);
}

} // namespace redasm::pm
