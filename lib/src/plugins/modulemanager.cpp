#include "modulemanager.h"
#include <filesystem>
#include <spdlog/spdlog.h>
#include <vector>

#ifdef _WIN32
#include <winbase.h>
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace redasm::mm {

namespace fs = std::filesystem;

namespace {

constexpr const char PLUGIN_CREATE[] = "rdplugin_create";
constexpr const char PLUGIN_DESTROY[] = "rdplugin_destroy";

#ifdef _WIN32
using HModule = HMODULE;
#else
using HModule = void*;
#endif

using ModuleCreateFunc = void (*)();
using ModuleDestroyFunc = void (*)();

struct Module {
    HModule handle;
    ModuleCreateFunc createfn;
    ModuleDestroyFunc destroyfn;

    [[nodiscard]] bool create() const {
        if(createfn) {
            createfn();
            return true;
        }
        return false;
    }

    void destroy() const {
        if(destroyfn) destroyfn();
    }

    explicit operator bool() const { return !!this->handle; }
};

SearchPaths g_searchpaths;
std::vector<Module> g_modules;

template<typename Function>
Function get_function(HModule h, std::string_view name) {
#ifdef _WIN32
    return reinterpret_cast<Function>(GetProcAddress(h, name.data()));
#else
    return reinterpret_cast<Function>(dlsym(h, name.data()));
#endif
}

void unload(const Module& m) {
    if(!m) return;

#ifdef _WIN32
    FreeLibrary(m.handle);
#else
    dlclose(m.handle);
#endif
}

Module load(std::string_view modulepath) {
    spdlog::info("Loading Module '{}'", modulepath);
    Module m{};

#ifdef _WIN32
    m.handle = LoadLibraryA(modulepath.data());
#else
    m.handle = dlopen(modulepath.data(), RTLD_LAZY);
#endif

    if(m.handle) {
        // TODO(davide): Handle if-statement
    }
    else {
#ifdef _WIN32
        // TODO: Implement Win32 Error Message Handling
#else
        spdlog::error("Failed to load {}", modulepath);
        spdlog::error("{}", dlerror());
#endif
    }

    m.createfn = mm::get_function<ModuleCreateFunc>(m.handle, PLUGIN_CREATE);

    if(!m.createfn) {
        spdlog::error("{}: '{}' not found", modulepath, PLUGIN_CREATE);
        mm::unload(m);
        return {};
    }

    m.destroyfn = mm::get_function<ModuleDestroyFunc>(m.handle, PLUGIN_DESTROY);

    return m;
}

void load_all_from(const std::string& p) {
    if(fs::directory_entry e{p}; e.is_directory()) {
        for(const auto& entry : fs::recursive_directory_iterator{
                e, fs::directory_options::follow_directory_symlink}) {
            if(entry.is_directory() ||
               entry.path().extension() != SHARED_OBJECT_EXT)
                continue;

            Module mod = mm::load(entry.path().string());
            if(mod.create())
                g_modules.emplace_back(mod);
            else {
                mod.destroy();
                mm::unload(mod);
            }
        }
    }
}

} // namespace

const SearchPaths& get_searchpaths() { return g_searchpaths; }

void add_searchpath(const std::string& sp) {
    if(sp.empty()) return;

    auto it = std::ranges::find(g_searchpaths, sp);

    if(it == g_searchpaths.end())
        g_searchpaths.push_back(sp);
    else
        spdlog::warn("add_searchpath(): duplicate entry '{}'", sp);
}

void unload_all() {
    spdlog::info("Unloading Modules");
    for(const Module& m : g_modules) {
        m.destroy();
        unload(m);
    }

    g_modules.clear();
}

void load_all() {
    spdlog::info("Loading Modules");
    for(const std::string& p : g_searchpaths)
        mm::load_all_from(p);
}

} // namespace redasm::mm
