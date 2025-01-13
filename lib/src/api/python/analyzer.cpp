#include "analyzer.h"
#include "../internal/analyzer.h"
#include "common.h"
#include <redasm/loader.h>
#include <redasm/version.h>

namespace redasm::api::python {

struct RDPYAnalyzerPlugin {
    RDAnalyzerPlugin base;
    PyObject* pyclass;
};

PyObject* register_analyzer(PyObject* /*self*/, PyObject* args) {
    PyObject* pyclass = nullptr;

    if(!PyArg_ParseTuple(args, "O", &pyclass) ||
       !python::validate_class(pyclass, {"id", "name", "execute"}))
        return nullptr;

    PyObject* idattr = PyObject_GetAttrString(pyclass, "id");
    PyObject* nameattr = PyObject_GetAttrString(pyclass, "name");

    PyObject* flagsattr = nullptr;
    if(PyObject_HasAttrString(pyclass, "flags"))
        flagsattr = PyObject_GetAttrString(pyclass, "flags");

    auto* plugin = new RDPYAnalyzerPlugin{};
    plugin->pyclass = pyclass;
    plugin->base.level = REDASM_API_LEVEL;
    plugin->base.id = PyUnicode_AsUTF8(idattr);
    plugin->base.name = PyUnicode_AsUTF8(nameattr);
    plugin->base.flags = flagsattr ? PyLong_AsUnsignedLong(flagsattr) : 0;

    plugin->base.on_init = [](const RDAnalyzerPlugin* arg) {
        const auto* plugin = reinterpret_cast<const RDPYAnalyzerPlugin*>(arg);
        Py_INCREF(plugin->pyclass);
    };

    plugin->base.on_shutdown = [](const RDAnalyzerPlugin* arg) {
        const auto* plugin = reinterpret_cast<const RDPYAnalyzerPlugin*>(arg);
        Py_DECREF(plugin->pyclass);
    };

    plugin->base.create = [](const RDAnalyzerPlugin* arg) -> RDAnalyzer* {
        const auto* plugin = reinterpret_cast<const RDPYAnalyzerPlugin*>(arg);
        PyObject* self = PyObject_CallNoArgs(plugin->pyclass);
        if(self) return reinterpret_cast<RDAnalyzer*>(self);
        python::check_error();
        return nullptr;
    };

    plugin->base.destroy = [](RDAnalyzer* arg) -> void {
        Py_DECREF(reinterpret_cast<PyObject*>(arg));
    };

    plugin->base.execute = [](RDAnalyzer* arg) {
        auto* self = reinterpret_cast<PyObject*>(arg);
        PyObject* res = PyObject_CallMethod(self, "execute", nullptr);
        python::check_error();
        Py_XDECREF(res);
    };

    if(PyObject_HasAttrString(pyclass, "is_enabled")) {
        plugin->base.is_enabled = [](const RDAnalyzerPlugin* arg) -> bool {
            const auto* plugin =
                reinterpret_cast<const RDPYAnalyzerPlugin*>(arg);
            PyObject* res =
                PyObject_CallMethod(plugin->pyclass, "is_enabled", nullptr);

            if(res) {
                bool ok = Py_IsTrue(res);
                Py_DECREF(res);
                return ok;
            }

            python::check_error();
            return false;
        };
    }

    return PyBool_FromLong(internal::register_analyzer(
        reinterpret_cast<RDAnalyzerPlugin*>(plugin), pm::Origin::PYTHON));
}

} // namespace redasm::api::python
