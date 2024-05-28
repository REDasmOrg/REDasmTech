#include "typing.h"
#include "../internal/typing.h"

namespace redasm::api::python {

PyObject* size_of(PyObject* /*self*/, PyObject* args) {
    const char* tname = PyUnicode_AsUTF8(args);
    return Py_BuildValue("K", internal::size_of(tname));
}

PyObject* create_struct(PyObject* /*self*/, PyObject* args) {
    const char* name = nullptr;
    PyObject* fieldsobj = nullptr;

    if(!PyArg_ParseTuple(args, "sO", &name, &fieldsobj))
        return nullptr;

    if(!PyTuple_Check(fieldsobj)) {
        PyErr_SetString(PyExc_TypeError, "Expected a tuple");
        return nullptr;
    }

    Py_ssize_t size = PyTuple_Size(fieldsobj);

    typing::StructBody fields;
    fields.reserve(size);

    for(Py_ssize_t i = 0; i < size; ++i) {
        PyObject* field = PyTuple_GetItem(fieldsobj, i);

        if(!PyTuple_Check(field)) {
            PyErr_SetString(PyExc_TypeError, "Expected a tuple");
            return nullptr;
        }

        if(auto s = PyTuple_Size(field); s != 2) {
            PyErr_SetString(PyExc_TypeError, "Invalid tuple size");
            return nullptr;
        }

        const char* ftype = PyUnicode_AsUTF8(PyTuple_GetItem(field, 0));
        const char* fname = PyUnicode_AsUTF8(PyTuple_GetItem(field, 1));
        fields.emplace_back(ftype, fname);
    }

    return Py_BuildValue("s", internal::create_struct(name, fields).c_str());
}

} // namespace redasm::api::python
