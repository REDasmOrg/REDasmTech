#pragma once

#include <Python.h>

namespace redasm::api::python {

PyObject* register_analyzer(PyObject* self, PyObject* args);

}
