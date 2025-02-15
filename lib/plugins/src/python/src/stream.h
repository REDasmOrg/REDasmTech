#pragma once

#include <Python.h>

namespace python {

extern PyTypeObject stream_type;

PyObject* pystream_new();

} // namespace python
