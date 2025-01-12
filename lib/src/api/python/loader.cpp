#include "loader.h"
#include "../../error.h"
#include "../../plugins/pluginmanager.h"
#include "../internal/loader.h"
#include "buffer.h"
#include "common.h"
#include "plugin.h"
#include <redasm/loader.h>

namespace redasm::api::python {

struct RDPYLoaderPlugin {
    RDLoaderPlugin base;
    PyObject* pyclass;
};

namespace {

PyObject* loader_accept(PyObject* self, PyObject* args) {
    const auto* plugin = plugin::get_bind<RDLoaderPlugin>(self);
    assume(plugin);
    bool b = false;

    if(plugin->load) {
        if(pm::get_origin(plugin) == pm::Origin::PYTHON) {
            RDLoaderRequest req = python::loadrequest_fromobject(args);
            b = plugin->accept(plugin, &req);
        }
        else {
            b = plugin->accept(plugin,
                               reinterpret_cast<RDLoaderRequest*>(
                                   PyCapsule_GetPointer(args, nullptr)));
        }
    }

    return PyBool_FromLong(b);
}

PyObject* loader_load(PyObject* self, PyObject* args) {
    using Instance = typename pm::InstanceForPlugin<RDLoaderPlugin>::Type;
    const auto* plugin = plugin::get_bind<RDLoaderPlugin>(self);
    assume(plugin);
    bool b = false;

    if(plugin->load) {
        if(pm::get_origin(plugin) == pm::Origin::PYTHON)
            b = plugin->load(reinterpret_cast<Instance*>(args));
        else {
            b = plugin->load(reinterpret_cast<Instance*>(
                PyCapsule_GetPointer(args, nullptr)));
        }
    }

    return PyBool_FromLong(b);
}

// clang-format off
PyMethodDef loader_methods[] = {
    {"on_init", reinterpret_cast<PyCFunction>(python::plugin::on_init<RDLoaderPlugin>), METH_NOARGS, nullptr},
    {"on_shutdown", reinterpret_cast<PyCFunction>(python::plugin::on_shutdown<RDLoaderPlugin>), METH_NOARGS, nullptr},
    {"create", reinterpret_cast<PyCFunction>(python::plugin::create<RDLoaderPlugin>), METH_NOARGS, nullptr},
    {"destroy", reinterpret_cast<PyCFunction>(python::plugin::destroy<RDLoaderPlugin>), METH_O, nullptr},
    {"accept", reinterpret_cast<PyCFunction>(python::loader_accept), METH_O, nullptr},
    {"load", reinterpret_cast<PyCFunction>(python::loader_load), METH_O, nullptr},
    {nullptr},
};
// clang-format on

} // namespace

PyObject* register_loader(PyObject* /*self*/, PyObject* args) {
    PyObject* pyclass = nullptr;

    if(!PyArg_ParseTuple(args, "O", &pyclass) ||
       !python::validate_class(pyclass, {"id", "name", "accept", "load"}))
        return nullptr;

    PyObject* idattr = PyObject_GetAttrString(pyclass, "id");
    PyObject* nameattr = PyObject_GetAttrString(pyclass, "name");

    PyObject* flagsattr = nullptr;
    if(PyObject_HasAttrString(pyclass, "flags"))
        flagsattr = PyObject_GetAttrString(pyclass, "flags");

    auto* plugin = new RDPYLoaderPlugin{};
    plugin->pyclass = pyclass;
    plugin->base.id = PyUnicode_AsUTF8(idattr);
    plugin->base.name = PyUnicode_AsUTF8(nameattr);
    plugin->base.flags = flagsattr ? PyLong_AsUnsignedLong(flagsattr) : 0;

    plugin->base.on_init = [](const RDLoaderPlugin* arg) {
        const auto* plugin = reinterpret_cast<const RDPYLoaderPlugin*>(arg);
        Py_INCREF(plugin->pyclass);
    };

    plugin->base.on_shutdown = [](const RDLoaderPlugin* arg) {
        const auto* plugin = reinterpret_cast<const RDPYLoaderPlugin*>(arg);
        Py_DECREF(plugin->pyclass);
    };

    plugin->base.create = [](const RDLoaderPlugin* arg) -> RDLoader* {
        const auto* plugin = reinterpret_cast<const RDPYLoaderPlugin*>(arg);
        PyObject* self = PyObject_CallNoArgs(plugin->pyclass);
        if(self) return reinterpret_cast<RDLoader*>(self);
        python::check_error();
        return nullptr;
    };

    plugin->base.destroy = [](RDLoader* arg) -> void {
        Py_DECREF(reinterpret_cast<PyObject*>(arg));
    };

    plugin->base.accept = [](const RDLoaderPlugin* arg,
                             const RDLoaderRequest* req) -> bool {
        const auto* plugin = reinterpret_cast<const RDPYLoaderPlugin*>(arg);
        PyObject* res = PyObject_CallMethod(plugin->pyclass, "accept", "O",
                                            python::loadrequest_toobject(req));

        if(res) {
            bool ok = Py_IsTrue(res);
            Py_DECREF(res);
            return ok;
        }

        python::check_error();
        return false;
    };

    plugin->base.load = [](RDLoader* arg) -> bool {
        auto* self = reinterpret_cast<PyObject*>(arg);
        PyObject* res = PyObject_CallMethod(self, "load", nullptr);

        if(res) {
            bool ok = Py_IsTrue(res);
            Py_DECREF(res);
            return ok;
        }

        python::check_error();
        return false;
    };

    return PyBool_FromLong(internal::register_loader(
        reinterpret_cast<RDLoaderPlugin*>(plugin), pm::Origin::PYTHON));
}

PyObject* get_loaderplugin(PyObject* /*self*/, PyObject* /*args*/) {
    const RDLoaderPlugin* plugin = internal::get_loaderplugin();
    assume(plugin);
    auto* res = python::new_simplenamespace();
    python::plugin::bind_default(res, plugin);
    python::plugin::bind_to(res, plugin);
    python::attach_methods(res, python::loader_methods);
    return res;
}

PyObject* get_loader(PyObject* /*self*/, PyObject* /*args*/) {
    if(RDLoader* l = internal::get_loader(); l) {
        const RDLoaderPlugin* plugin = internal::get_loaderplugin();
        assume(plugin);
        if(pm::get_origin(plugin) == pm::Origin::PYTHON)
            return reinterpret_cast<PyObject*>(l);
        return PyCapsule_New(l, nullptr, nullptr);
    }

    return Py_None;
}

} // namespace redasm::api::python
