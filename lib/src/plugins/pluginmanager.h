#pragma once

#include "../error.h"
#include <redasm/redasm.h>
#include <string_view>

namespace redasm::pm {

#define _foreach_plugins(type, item, body)                                     \
    do {                                                                       \
        usize n;                                                               \
        const auto** plugins = pm::get_##type##s(&n);                          \
        for(usize i = 0; i < n; i++) {                                         \
            const auto* item = plugins[i];                                     \
            body                                                               \
        }                                                                      \
    } while(0);

// clang-format off
#define foreach_loaders(item, body) _foreach_plugins(loader, item, body)
#define foreach_processors(item, body) _foreach_plugins(processor, item, body)
#define foreach_analyzers(item, body) _foreach_plugins(analyzer, item, body)
// clang-format on

namespace impl {

// clang-format off
template<typename T> struct InstanceForPlugin {};
template<> struct InstanceForPlugin<RDLoaderPlugin> { using Type = RDLoader; };
template<> struct InstanceForPlugin<RDProcessorPlugin> { using Type = RDProcessor; };
template<> struct InstanceForPlugin<RDAnalyzerPlugin> { using Type = RDAnalyzer; };
// clang-format on

} // namespace impl

void create();
void destroy();

bool register_loader(const RDLoaderPlugin* plugin);
bool register_processor(const RDProcessorPlugin* plugin);
bool register_analyzer(const RDAnalyzerPlugin* plugin);

const RDLoaderPlugin** get_loaders(usize* n);
const RDProcessorPlugin** get_processors(usize* n);
const RDAnalyzerPlugin** get_analyzers(usize* n);

const RDLoaderPlugin* find_loader(std::string_view id);
const RDProcessorPlugin* find_processor(std::string_view id);
const RDAnalyzerPlugin* find_analyzer(std::string_view id);

template<typename T>
impl::InstanceForPlugin<T>::Type* create_instance(const T* plugin) {
    assume(plugin);
    using Instance = impl::InstanceForPlugin<T>::Type;

    Instance* instance = nullptr;
    if(plugin->create)
        instance = reinterpret_cast<Instance*>(plugin->create(plugin));
    return instance;
}

template<typename T>
void destroy_instance(const T* plugin,
                      typename impl::InstanceForPlugin<T>::Type* p) {
    if(!plugin && !p) return;
    assume(plugin);
    if(plugin->destroy && p) plugin->destroy(p);
}

} // namespace redasm::pm
