#include "../internal/byteorder.h"
#include "byteorder.h"
#include "common.h"

namespace redasm::api::python {

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
