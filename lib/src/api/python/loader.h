#pragma once

#include <Python.h>

namespace redasm::api::python {

PyObject* register_loader(PyObject* self, PyObject* args);
PyObject* get_loaderplugin(PyObject* self, PyObject* args);
PyObject* get_loader(PyObject* self, PyObject* args);

} // namespace redasm::api::python
