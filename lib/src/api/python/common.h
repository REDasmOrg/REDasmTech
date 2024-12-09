#pragma once

#include "../../typing/typing.h"
#include <Python.h>
#include <string_view>

namespace redasm::api::python {

inline PyCFunction to_cfunction(PyCFunctionWithKeywords f) {
    return reinterpret_cast<PyCFunction>(reinterpret_cast<void*>(f));
}

PyObject* new_simplenamespace();
PyObject* to_object(const typing::Value& v);
void check_error();

inline PyObject* type_error(PyObject* obj, std::string_view expected) {
    PyErr_Format(PyExc_TypeError, "Expected type '%.*s', got '%s'",
                 static_cast<int>(expected.size()), expected.data(),
                 Py_TYPE(obj)->tp_name);
    return nullptr;
}

} // namespace redasm::api::python
