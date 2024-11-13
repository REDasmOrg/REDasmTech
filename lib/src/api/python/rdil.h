#pragma once

#include <Python.h>

namespace redasm::api::python {

extern PyTypeObject rdil_type;

PyObject* pyrdil_new();

} // namespace redasm::api::python
