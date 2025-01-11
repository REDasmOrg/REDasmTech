#include "typing.h"
#include "../internal/typing.h"
#include "common.h"

namespace redasm::api::python {

PyObject* size_of(PyObject* /*self*/, PyObject* args) {
    const char* tname = PyUnicode_AsUTF8(args);
    return PyLong_FromUnsignedLongLong(internal::size_of(tname));
}

PyObject* create_struct(PyObject* /*self*/, PyObject* args) {
    const char* name = nullptr;
    PyObject* obj = nullptr;

    if(!PyArg_ParseTuple(args, "sO", &name, &obj)) return nullptr;

    typing::Struct s;
    if(!python::tuple_to_struct(obj, s)) return nullptr;
    return PyUnicode_FromString(internal::create_struct(name, s).c_str());
}

} // namespace redasm::api::python
