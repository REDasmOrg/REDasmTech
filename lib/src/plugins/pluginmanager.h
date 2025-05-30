#pragma once

#include <redasm/redasm.h>
#include <string_view>

namespace redasm::pm {

constexpr const char* NATIVE = "native";
inline RDLoaderPluginSlice loaders;
inline RDProcessorPluginSlice processors;
inline RDAnalyzerPluginSlice analyzers;

// clang-format off
template<typename T> struct InstanceForPlugin {};
template<> struct InstanceForPlugin<RDLoaderPlugin> { using Type = RDLoader; };
template<> struct InstanceForPlugin<RDProcessorPlugin> { using Type = RDProcessor; };
template<> struct InstanceForPlugin<RDAnalyzerPlugin> { using Type = RDAnalyzer; };
// clang-format on

void create();
void destroy();
const char* get_origin(const void* plugin);
bool is_origin(const void* plugin, const char* origin);

bool register_loader(const RDLoaderPlugin* plugin, const char* origin);
bool register_processor(const RDProcessorPlugin* plugin, const char* origin);
bool register_analyzer(const RDAnalyzerPlugin* plugin, const char* origin);

const RDLoaderPlugin* find_loader(std::string_view id);
const RDProcessorPlugin* find_processor(std::string_view id);
const RDAnalyzerPlugin* find_analyzer(std::string_view id);

template<typename T>
pm::InstanceForPlugin<T>::Type* create_instance(const T* plugin) {
    ct_assume(plugin);
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
    ct_assume(plugin);
    if(plugin->destroy && p) plugin->destroy(p);
}

} // namespace redasm::pm
