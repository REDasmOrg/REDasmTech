#pragma once

#include <Python.h>

namespace redasm::api::python {

extern PyTypeObject abstractstream_type;
extern PyTypeObject filestream_type;
extern PyTypeObject memorystream_type;

PyObject* pyfilestream_new();
PyObject* pymemorystream_new();

} // namespace redasm::api::python
