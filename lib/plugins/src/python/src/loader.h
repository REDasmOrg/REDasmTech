#pragma once

#include <Python.h>

namespace python {

PyObject* register_loader(PyObject* self, PyObject* args);
PyObject* get_loaderplugin(PyObject* self, PyObject* args);
PyObject* get_loader(PyObject* self, PyObject* args);

} // namespace python
