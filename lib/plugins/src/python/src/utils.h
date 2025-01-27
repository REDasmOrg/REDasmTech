#pragma once

#include <Python.h>
#include <redasm/redasm.h>

namespace python {

PyObject* to_hex(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rol16(PyObject* self, PyObject* args);
PyObject* rol32(PyObject* self, PyObject* args);
PyObject* rol64(PyObject* self, PyObject* args);
PyObject* ror16(PyObject* self, PyObject* args);
PyObject* ror32(PyObject* self, PyObject* args);
PyObject* ror64(PyObject* self, PyObject* args);

} // namespace python
