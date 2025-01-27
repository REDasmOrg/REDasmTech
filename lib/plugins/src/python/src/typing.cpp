#include "typing.h"
#include "common.h"
#include <redasm/typing.h>

namespace python {

PyObject* size_of(PyObject* /*self*/, PyObject* args) {
    const char* tname = PyUnicode_AsUTF8(args);
    return PyLong_FromUnsignedLongLong(rd_nsizeof(tname));
}

PyObject* create_struct(PyObject* /*self*/, PyObject* args) {
    const char* name = nullptr;
    PyObject* obj = nullptr;
    if(!PyArg_ParseTuple(args, "sO", &name, &obj)) return nullptr;

    std::vector<RDStructField> s;
    if(!python::tuple_to_struct(obj, s)) return nullptr;
    return PyUnicode_FromString(rd_createstruct(name, s.data()));
}

} // namespace python
