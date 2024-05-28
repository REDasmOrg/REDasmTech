#pragma once

#include "../../typing/typing.h"
#include <Python.h>

namespace redasm::api::python {

inline PyCFunction to_cfunction(PyCFunctionWithKeywords f) {
    return reinterpret_cast<PyCFunction>(reinterpret_cast<void*>(f));
}

PyObject* new_simplenamespace();
PyObject* to_object(const typing::Value& v);
void on_error();

} // namespace redasm::api::python
