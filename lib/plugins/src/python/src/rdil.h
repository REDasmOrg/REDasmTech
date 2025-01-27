#pragma once

#include <Python.h>

namespace python {

extern PyTypeObject rdil_type;

PyObject* pyrdil_new();

} // namespace python
