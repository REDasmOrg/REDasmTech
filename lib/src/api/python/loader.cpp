#include "loader.h"
#include "../internal/loader.h"
#include "common.h"
#include <redasm/loader.h>

namespace redasm::api::python {

struct RDPYLoaderPlugin {
    RDLoaderPlugin base;
    PyObject* pyclass;
};

PyObject* register_loader(PyObject* /*self*/, PyObject* args) {
    PyObject* pyclass = nullptr;

    if(!PyArg_ParseTuple(args, "O", &pyclass) ||
       !python::validate_class(pyclass, {"id", "name", "flags", "load"}))
        return nullptr;

    PyObject* idattr = PyObject_GetAttrString(pyclass, "id");
    PyObject* nameattr = PyObject_GetAttrString(pyclass, "name");
    PyObject* flagsattr = PyObject_GetAttrString(pyclass, "flags");

    auto* plugin = new RDPYLoaderPlugin{};
    plugin->pyclass = pyclass;
    plugin->base.id = PyUnicode_AsUTF8(idattr);
    plugin->base.name = PyUnicode_AsUTF8(nameattr);
    plugin->base.flags = PyLong_AsUnsignedLong(flagsattr);

    plugin->base.oninit = [](const RDLoaderPlugin* arg) {
        const auto* plugin = reinterpret_cast<const RDPYLoaderPlugin*>(arg);
        Py_INCREF(plugin->pyclass);
    };

    plugin->base.onshutdown = [](const RDLoaderPlugin* arg) {
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

    plugin->base.load = [](RDLoader* arg) -> bool {
        auto* self = reinterpret_cast<PyObject*>(arg);
        PyObject* res = PyObject_CallMethod(self, "load", nullptr);

        if(res) {
            bool ok = PyBool_Check(res);
            Py_DECREF(res);
            return ok;
        }

        python::check_error();
        return false;
    };

    return PyBool_FromLong(
        internal::register_loader(reinterpret_cast<RDLoaderPlugin*>(plugin)));
}

} // namespace redasm::api::python
