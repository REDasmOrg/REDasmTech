#pragma once

#include <Python.h>

namespace python {

PyObject* size_of(PyObject* self, PyObject* args);
PyObject* create_struct(PyObject* self, PyObject* args);

} // namespace python
