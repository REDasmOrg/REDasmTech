#pragma once

#include <Python.h>

namespace redasm::api::python {

extern PyTypeObject file_type;
extern PyTypeObject memory_type;

PyObject* pyfile_new();
PyObject* pymemory_new();

} // namespace redasm::api::python
