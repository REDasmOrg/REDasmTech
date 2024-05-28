#pragma once

#include <Python.h>

namespace redasm::api::python {

PyObject* size_of(PyObject* self, PyObject* args);
PyObject* create_struct(PyObject* self, PyObject* args);

} // namespace redasm::api::python
