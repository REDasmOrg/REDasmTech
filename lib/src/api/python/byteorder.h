#pragma once

#include <Python.h>

namespace redasm::api::python {

PyObject* from_le16(PyObject* self, PyObject* args);
PyObject* from_le32(PyObject* self, PyObject* args);
PyObject* from_le64(PyObject* self, PyObject* args);
PyObject* from_be16(PyObject* self, PyObject* args);
PyObject* from_be32(PyObject* self, PyObject* args);
PyObject* from_be64(PyObject* self, PyObject* args);
PyObject* to_le16(PyObject* self, PyObject* args);
PyObject* to_le32(PyObject* self, PyObject* args);
PyObject* to_le64(PyObject* self, PyObject* args);
PyObject* to_be16(PyObject* self, PyObject* args);
PyObject* to_be32(PyObject* self, PyObject* args);
PyObject* to_be64(PyObject* self, PyObject* args);

} // namespace redasm::api::python
