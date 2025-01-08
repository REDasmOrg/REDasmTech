#pragma once

#include "../../plugins/pluginmanager.h"
#include <Python.h>

namespace redasm::api::python::plugin {

namespace impl {

constexpr const char* PLUGIN_KEY = "__plugin";
constexpr const char* PLUGIN_NAME = "Plugin";
} // namespace impl

template<typename Plugin>
void bind_default(PyObject* obj, const Plugin* plugin) {
    // clang-format off
    PyObject_SetAttrString(obj, "level", PyLong_FromUnsignedLong(plugin->level));
    PyObject_SetAttrString(obj, "id", PyUnicode_FromString(plugin->id));
    PyObject_SetAttrString(obj, "name", PyUnicode_FromString(plugin->name));
    PyObject_SetAttrString(obj, "flags", PyLong_FromUnsignedLong(plugin->flags));
    // clang-format on
}

template<typename Plugin>
void bind_to(PyObject* obj, const Plugin* plugin) {
    PyObject_SetAttrString(
        obj, impl::PLUGIN_KEY,
        PyCapsule_New(const_cast<Plugin*>(plugin), impl::PLUGIN_NAME, nullptr));
}

template<typename Plugin>
const Plugin* get_bind(PyObject* obj) {
    PyObject* capsule = PyObject_GetAttrString(obj, impl::PLUGIN_KEY);
    return reinterpret_cast<const Plugin*>(
        PyCapsule_GetPointer(capsule, impl::PLUGIN_NAME));
}

template<typename T>
PyObject* on_init(PyObject* self, PyObject* /*args*/) {
    const T* plugin = plugin::get_bind<T>(self);
    assume(plugin);
    if(plugin->on_init) plugin->on_init(plugin);
    return Py_None;
}

template<typename T>
PyObject* on_shutdown(PyObject* self, PyObject* /*args*/) {
    const T* plugin = plugin::get_bind<T>(self);
    assume(plugin);
    if(plugin->on_shutdown) plugin->on_shutdown(plugin);
    return Py_None;
}

template<typename T>
PyObject* create(PyObject* self, PyObject* /*args*/) {
    using Instance = typename pm::InstanceForPlugin<T>::Type;
    const T* plugin = plugin::get_bind<T>(self);
    assume(plugin);

    if(plugin->create) {
        Instance* instance = plugin->create(plugin);

        if(pm::get_origin(plugin) == pm::Origin::PYTHON)
            return reinterpret_cast<PyObject*>(instance);
        return PyCapsule_New(instance, nullptr, nullptr);
    }

    return Py_None;
}

template<typename T>
PyObject* destroy(PyObject* self, PyObject* args) {
    using Instance = typename pm::InstanceForPlugin<T>::Type;
    const T* plugin = plugin::get_bind<T>(self);
    assume(plugin);

    if(plugin->destroy) {
        if(pm::get_origin(plugin) == pm::Origin::PYTHON)
            plugin->destroy(reinterpret_cast<Instance*>(args));
        else {
            plugin->destroy(reinterpret_cast<Instance*>(
                PyCapsule_GetPointer(args, nullptr)));
        }
    }

    return Py_None;
};

} // namespace redasm::api::python::plugin
