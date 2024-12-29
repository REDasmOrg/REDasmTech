#pragma once

#include <Python.h>

namespace redasm::api::python {

PyObject* to_hex(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rol16(PyObject* self, PyObject* args);
PyObject* rol32(PyObject* self, PyObject* args);
PyObject* rol64(PyObject* self, PyObject* args);
PyObject* ror16(PyObject* self, PyObject* args);
PyObject* ror32(PyObject* self, PyObject* args);
PyObject* ror64(PyObject* self, PyObject* args);

} // namespace redasm::api::python
