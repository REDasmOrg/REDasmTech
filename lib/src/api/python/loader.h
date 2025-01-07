#pragma once

#include <Python.h>

namespace redasm::api::python {

PyObject* register_loader(PyObject* self, PyObject* args);

}
