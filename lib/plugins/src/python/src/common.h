#pragma once

#include <Python.h>
#include <redasm/redasm.h>
#include <string_view>
#include <vector>

namespace python {

constexpr const char* ID = "python";

inline PyCFunction to_cfunction(PyCFunctionWithKeywords f) {
    return reinterpret_cast<PyCFunction>(reinterpret_cast<void*>(f));
}

bool validate_class(PyObject* obj, std::initializer_list<const char*> reqattrs);
bool tuple_to_struct(PyObject* obj, std::vector<RDStructFieldDecl>& s);
PyObject* new_simplenamespace();
PyObject* to_object(const RDValue* v);
void attach_methods(PyObject* obj, PyMethodDef* methods);
void check_error();
PyObject* loadrequest_toobject(const RDLoaderRequest* req);
RDLoaderRequest loadrequest_fromobject(PyObject* obj);

inline PyObject* attr_error(PyObject* obj, std::string_view expected) {
    PyErr_Format(PyExc_TypeError, "Missing attribute '%.*s', in object '%s'",
                 static_cast<int>(expected.size()), expected.data(),
                 Py_TYPE(obj)->tp_name);
    return nullptr;
}

inline PyObject* type_error(PyObject* obj, std::string_view expected) {
    PyErr_Format(PyExc_TypeError, "Expected type '%.*s', got '%s'",
                 static_cast<int>(expected.size()), expected.data(),
                 Py_TYPE(obj)->tp_name);
    return nullptr;
}

} // namespace python
