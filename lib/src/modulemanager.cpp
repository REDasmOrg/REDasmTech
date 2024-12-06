#include "modulemanager.h"
#include "error.h"
#include <filesystem>
#include <spdlog/spdlog.h>
#include <vector>

#ifdef _WIN32
#include <winbase.h>
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace redasm {

namespace fs = std::filesystem;

namespace {

constexpr const char PLUGIN_INIT[] = "rdplugin_init";
constexpr const char PLUGIN_FREE[] = "rdplugin_free";

#ifdef _WIN32
using HModule = HMODULE;
#else
using HModule = void*;
#endif

using ModuleInitFunc = void (*)();
using ModuleDeinitFunc = void (*)();

struct Module {
    HModule handle;
    ModuleInitFunc initfn;
    ModuleDeinitFunc deinitfn;

    void init() const {
        assume(initfn);
        initfn();
    }

    void deinit() const {
        if(deinitfn)
            deinitfn();
    }

    explicit operator bool() const { return !!this->handle; }
};

SearchPaths g_searchpaths;
std::vector<Module> g_modules;

template<typename Function>
Function get_module_function(HModule h, std::string_view name) {
#ifdef _WIN32
    return reinterpret_cast<Function>(GetProcAddress(h, name.data()));
#else
    return reinterpret_cast<Function>(dlsym(h, name.data()));
#endif
}

void unload_module(const Module& m) {
    if(!m)
        return;

#ifdef _WIN32
    FreeLibrary(m.handle);
#else
    dlclose(m.handle);
#endif
}

Module load_module(std::string_view modulepath) {
    spdlog::info("Loading Module '{}'", modulepath);
    Module m{};

#ifdef _WIN32
    m.handle = LoadLibraryA(modulepath.data());
#else
    m.handle = dlopen(modulepath.data(), RTLD_LAZY);
#endif

    if(m.handle) {
    }
    else {
#ifdef _WIN32
        // TODO: Implement Win32 Error Message Handling
#else
        spdlog::error("Failed to load {}", modulepath);
        spdlog::error("{}", dlerror());
#endif
    }

    m.initfn = get_module_function<ModuleInitFunc>(m.handle, PLUGIN_INIT);

    if(!m.initfn) {
        spdlog::error("{}: '{}' not found", modulepath, PLUGIN_INIT);
        unload_module(m);
        return {};
    }

    m.deinitfn = get_module_function<ModuleDeinitFunc>(m.handle, PLUGIN_FREE);

    return m;
}

void load_modules_from(const std::string& p) {
    if(fs::directory_entry e{p}; e.is_directory()) {
        for(const auto& entry : fs::recursive_directory_iterator{
                e, fs::directory_options::follow_directory_symlink}) {
            if(entry.is_directory() ||
               entry.path().extension() != SHARED_OBJECT_EXT)
                continue;

            g_modules.emplace_back(load_module(entry.path().string()));
            g_modules.back().init();
        }
    }
}

} // namespace

const SearchPaths& get_searchpaths() { return g_searchpaths; }

void add_searchpath(const std::string& sp) {
    if(sp.empty())
        return;

    auto it = std::ranges::find(g_searchpaths, sp);

    if(it == g_searchpaths.end())
        g_searchpaths.push_back(sp);
    else
        spdlog::warn("add_searchpath(): duplicate entry '{}'", sp);
}

void unload_modules() {
    for(const Module& m : g_modules) {
        m.deinit();
        unload_module(m);
    }

    g_modules.clear();
}

void load_modules() {
    for(const std::string& p : g_searchpaths)
        redasm::load_modules_from(p);
}

} // namespace redasm
