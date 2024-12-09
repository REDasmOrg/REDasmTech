#include "../internal/utils.h"
#include "common.h"
#include "utils.h"

namespace redasm::api::python {

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

    if(n > 0)
        return PyUnicode_FromString(internal::to_hex_n(v, n).c_str());

    return PyUnicode_FromString(internal::to_hex(v).c_str());
}

PyObject* rol16(PyObject* /*self*/, PyObject* args) {
    u16 val = 0, amt = 0;

    if(!PyArg_ParseTuple(args, "HH", &args))
        return nullptr;

    return PyLong_FromUnsignedLong(internal::rol16(val, amt));
}

PyObject* rol32(PyObject* /*self*/, PyObject* args) {
    u32 val = 0, amt = 0;

    if(!PyArg_ParseTuple(args, "kk", &args))
        return nullptr;

    return PyLong_FromUnsignedLong(internal::rol32(val, amt));
}

PyObject* rol64(PyObject* /*self*/, PyObject* args) {
    u64 val = 0, amt = 0;

    if(!PyArg_ParseTuple(args, "KK", &args))
        return nullptr;

    return PyLong_FromUnsignedLong(internal::rol64(val, amt));
}

PyObject* ror16(PyObject* /*self*/, PyObject* args) {
    u16 val = 0, amt = 0;

    if(!PyArg_ParseTuple(args, "HH", &args))
        return nullptr;

    return PyLong_FromUnsignedLong(internal::ror16(val, amt));
}

PyObject* ror32(PyObject* /*self*/, PyObject* args) {
    u32 val = 0, amt = 0;

    if(!PyArg_ParseTuple(args, "kk", &args))
        return nullptr;

    return PyLong_FromUnsignedLong(internal::ror32(val, amt));
}

PyObject* ror64(PyObject* /*self*/, PyObject* args) {
    u64 val = 0, amt = 0;

    if(!PyArg_ParseTuple(args, "KK", &args))
        return nullptr;

    return PyLong_FromUnsignedLong(internal::ror64(val, amt));
}

PyObject* from_le16(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        python::type_error(args, "int");

    u16 hostval = PyLong_AsUnsignedLong(args);
    return PyLong_FromUnsignedLong(internal::from_le16(hostval));
}

PyObject* from_le32(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        python::type_error(args, "int");

    u32 hostval = PyLong_AsUnsignedLong(args);
    return PyLong_FromUnsignedLong(internal::from_le32(hostval));
}

PyObject* from_le64(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        python::type_error(args, "int");

    u64 hostval = PyLong_AsUnsignedLongLong(args);
    return PyLong_FromUnsignedLongLong(internal::from_le64(hostval));
}

PyObject* from_be16(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        python::type_error(args, "int");

    u16 hostval = PyLong_AsUnsignedLong(args);
    return PyLong_FromUnsignedLong(internal::from_be16(hostval));
}

PyObject* from_be32(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        python::type_error(args, "int");

    u32 hostval = PyLong_AsUnsignedLong(args);
    return PyLong_FromUnsignedLong(internal::from_be32(hostval));
}

PyObject* from_be64(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        python::type_error(args, "int");

    u64 hostval = PyLong_AsUnsignedLongLong(args);
    return PyLong_FromUnsignedLongLong(internal::from_be64(hostval));
}

PyObject* to_le16(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        python::type_error(args, "int");

    u16 hostval = PyLong_AsUnsignedLongLong(args);
    return PyLong_FromUnsignedLongLong(internal::to_le16(hostval));
}

PyObject* to_le32(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        python::type_error(args, "int");

    u32 hostval = PyLong_AsUnsignedLong(args);
    return PyLong_FromUnsignedLong(internal::to_le32(hostval));
}

PyObject* to_le64(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        python::type_error(args, "int");

    u64 hostval = PyLong_AsUnsignedLong(args);
    return PyLong_FromUnsignedLong(internal::to_le64(hostval));
}

PyObject* to_be16(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        python::type_error(args, "int");

    u16 hostval = PyLong_AsUnsignedLongLong(args);
    return PyLong_FromUnsignedLongLong(internal::to_be16(hostval));
}

PyObject* to_be32(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        python::type_error(args, "int");

    u32 hostval = PyLong_AsUnsignedLongLong(args);
    return PyLong_FromUnsignedLongLong(internal::to_be32(hostval));
}

PyObject* to_be64(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        python::type_error(args, "int");

    u64 hostval = PyLong_AsUnsignedLongLong(args);
    return PyLong_FromUnsignedLongLong(internal::to_be64(hostval));
}

} // namespace redasm::api::python
