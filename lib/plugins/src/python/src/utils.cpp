#include "utils.h"

namespace python {

PyObject* to_hex(PyObject* /*self*/, PyObject* args, PyObject* kwargs) {
    static const char* const KW_LIST[] = {
        "",
        "n",
        nullptr,
    };

    usize v = 0;
    int n = 0;

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "K|i",
                                    const_cast<char**>(KW_LIST), &v, &n)) {
        return nullptr;
    }

    if(n > 0) return PyUnicode_FromString(rd_tohex_n(v, n));
    return PyUnicode_FromString(rd_tohex(v));
}

PyObject* rol16(PyObject* /*self*/, PyObject* args) {
    unsigned short int val = 0, amt = 0;
    if(!PyArg_ParseTuple(args, "HH", &val, &amt)) return nullptr;
    return PyLong_FromUnsignedLong(rd_rol16(val, amt));
}

PyObject* rol32(PyObject* /*self*/, PyObject* args) {
    unsigned int val = 0, amt = 0;
    if(!PyArg_ParseTuple(args, "II", &val, &amt)) return nullptr;
    return PyLong_FromUnsignedLong(rd_rol32(val, amt));
}

PyObject* rol64(PyObject* /*self*/, PyObject* args) {
    unsigned long long val = 0, amt = 0;
    if(!PyArg_ParseTuple(args, "KK", &val, &amt)) return nullptr;
    return PyLong_FromUnsignedLong(rd_rol64(val, amt));
}

PyObject* ror16(PyObject* /*self*/, PyObject* args) {
    unsigned short int val = 0, amt = 0;
    if(!PyArg_ParseTuple(args, "HH", &val, &amt)) return nullptr;
    return PyLong_FromUnsignedLong(rd_ror16(val, amt));
}

PyObject* ror32(PyObject* /*self*/, PyObject* args) {
    unsigned int val = 0, amt = 0;
    if(!PyArg_ParseTuple(args, "II", &val, &amt)) return nullptr;
    return PyLong_FromUnsignedLong(rd_ror32(val, amt));
}

PyObject* ror64(PyObject* /*self*/, PyObject* args) {
    unsigned long long val = 0, amt = 0;
    if(!PyArg_ParseTuple(args, "KK", &val, &amt)) return nullptr;
    return PyLong_FromUnsignedLong(rd_ror64(val, amt));
}

} // namespace python
