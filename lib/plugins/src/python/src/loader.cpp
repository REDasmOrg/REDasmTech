#include "loader.h"
#include "buffer.h"
#include "plugin.h"
#include <redasm/loader.h>
#include <redasm/version.h>

namespace python {

struct RDPYLoaderPlugin {
    RDLoaderPlugin base;
    PyObject* pyclass;
};

namespace {

PyObject* loader_parse(PyObject* self, PyObject* args) {
    using Instance = typename plugin::InstanceForPlugin<RDLoaderPlugin>::Type;

    PyObject *pyinstance = nullptr, *file = nullptr;
    if(!PyArg_ParseTuple(args, "OO", &self, &file)) return nullptr;

    const auto* plugin = plugin::get_bind<RDLoaderPlugin>(self);
    if(!plugin) return nullptr;

    bool b = false;

    if(plugin->load) {
        if(rdplugin_isorigin(plugin, python::ID)) {
            RDLoaderRequest req = python::loadrequest_fromobject(args);
            b = plugin->parse(reinterpret_cast<Instance*>(pyinstance), &req);
        }
        else {
            b = plugin->parse(reinterpret_cast<Instance*>(
                                  PyCapsule_GetPointer(pyinstance, nullptr)),
                              reinterpret_cast<RDLoaderRequest*>(
                                  PyCapsule_GetPointer(args, nullptr)));
        }
    }

    return PyBool_FromLong(b);
}

PyObject* loader_load(PyObject* self, PyObject* args) {
    using Instance = typename plugin::InstanceForPlugin<RDLoaderPlugin>::Type;

    PyObject *pyinstance = nullptr, *file = nullptr;
    if(!PyArg_ParseTuple(args, "OO", &self, &file)) return nullptr;

    const auto* plugin = plugin::get_bind<RDLoaderPlugin>(self);
    if(!plugin) return nullptr;

    bool b = false;

    if(plugin->load) {
        if(rdplugin_isorigin(plugin, python::ID))
            b = plugin->load(reinterpret_cast<Instance*>(pyinstance),
                             python::pybuffer_asbuffer(file));
    }
    else {
        b = plugin->load(
            reinterpret_cast<Instance*>(
                PyCapsule_GetPointer(pyinstance, nullptr)),
            reinterpret_cast<RDBuffer*>(PyCapsule_GetPointer(file, nullptr)));
    }

    return PyBool_FromLong(b);
}

PyObject* loader_getprocessor(PyObject* self, PyObject* args) {
    using Instance = typename plugin::InstanceForPlugin<RDLoaderPlugin>::Type;
    const auto* plugin = plugin::get_bind<RDLoaderPlugin>(self);
    if(!plugin) return nullptr;

    if(plugin->get_processor) {
        const char* p = nullptr;

        if(rdplugin_isorigin(plugin, python::ID))
            p = plugin->get_processor(reinterpret_cast<Instance*>(args));
        else {
            p = plugin->get_processor(reinterpret_cast<Instance*>(
                PyCapsule_GetPointer(args, nullptr)));
        }

        if(p) return PyUnicode_FromString(p);
    }

    return Py_None;
}

// clang-format off
PyMethodDef loader_methods[] = {
    {"on_init", reinterpret_cast<PyCFunction>(python::plugin::on_init<RDLoaderPlugin>), METH_NOARGS, nullptr},
    {"on_shutdown", reinterpret_cast<PyCFunction>(python::plugin::on_shutdown<RDLoaderPlugin>), METH_NOARGS, nullptr},
    {"create", reinterpret_cast<PyCFunction>(python::plugin::create<RDLoaderPlugin>), METH_NOARGS, nullptr},
    {"destroy", reinterpret_cast<PyCFunction>(python::plugin::destroy<RDLoaderPlugin>), METH_O, nullptr},
    {"parse", reinterpret_cast<PyCFunction>(python::loader_parse), METH_O, nullptr},
    {"load", reinterpret_cast<PyCFunction>(python::loader_load), METH_VARARGS, nullptr},
    {"get_processor", reinterpret_cast<PyCFunction>(python::loader_getprocessor), METH_O, nullptr},
    {},
};
// clang-format on

} // namespace

PyObject* register_loader(PyObject* /*self*/, PyObject* args) {
    PyObject* pyclass = nullptr;

    if(!PyArg_ParseTuple(args, "O", &pyclass) ||
       !python::validate_class(pyclass, {"id", "name", "parse", "load"}))
        return nullptr;

    PyObject* idattr = PyObject_GetAttrString(pyclass, "id");
    PyObject* nameattr = PyObject_GetAttrString(pyclass, "name");

    PyObject* flagsattr = nullptr;
    if(PyObject_HasAttrString(pyclass, "flags"))
        flagsattr = PyObject_GetAttrString(pyclass, "flags");

    auto* plugin = new RDPYLoaderPlugin{};
    plugin->pyclass = pyclass;
    plugin->base.level = REDASM_API_LEVEL;
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

    plugin->base.parse = [](RDLoader* self,
                            const RDLoaderRequest* req) -> bool {
        auto* obj = reinterpret_cast<PyObject*>(self);
        PyObject* res = PyObject_CallMethod(obj, "parse", "O",
                                            python::loadrequest_toobject(req));

        if(res) {
            bool ok = Py_IsTrue(res);
            Py_DECREF(res);
            return ok;
        }

        python::check_error();
        return false;
    };

    plugin->base.load = [](RDLoader* arg, RDBuffer* file) -> bool {
        auto* self = reinterpret_cast<PyObject*>(arg);
        PyObject* res = PyObject_CallMethod(self, "load", "O",
                                            python::pybuffer_frombuffer(file));

        if(res) {
            bool ok = Py_IsTrue(res);
            Py_DECREF(res);
            return ok;
        }

        python::check_error();
        return false;
    };

    plugin->base.get_processor = [](RDLoader* arg) -> const char* {
        auto* self = reinterpret_cast<PyObject*>(arg);

        if(PyObject_HasAttrString(self, "get_processor")) {
            PyObject* res = PyObject_CallMethod(self, "get_processor", nullptr);

            if(res) {
                if(Py_IsNone(res)) return nullptr;
                return PyUnicode_AsUTF8(res);
            }

            python::check_error();
        }

        return nullptr;
    };

    return PyBool_FromLong(rd_registerloader_ex(
        reinterpret_cast<RDLoaderPlugin*>(plugin), python::ID));
}

PyObject* get_loaderplugin(PyObject* /*self*/, PyObject* /*args*/) {
    const RDLoaderPlugin* plugin = rd_getloaderplugin();
    if(!plugin) return nullptr;

    auto* res = python::new_simplenamespace();
    python::plugin::bind_default(res, plugin);
    python::plugin::bind_to(res, plugin);
    python::attach_methods(res, python::loader_methods);
    return res;
}

PyObject* get_loader(PyObject* /*self*/, PyObject* /*args*/) {
    if(RDLoader* l = rd_getloader(); l) {
        const RDLoaderPlugin* plugin = rd_getloaderplugin();
        if(!plugin) return nullptr;

        if(rdplugin_isorigin(plugin, python::ID))
            return reinterpret_cast<PyObject*>(l);
        return PyCapsule_New(l, nullptr, nullptr);
    }

    return Py_None;
}

} // namespace python
