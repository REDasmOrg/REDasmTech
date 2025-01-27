#include "byteorder.h"
#include "common.h"
#include <redasm/byteorder.h>

namespace python {

PyObject* from_le16(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    u16 hostval = PyLong_AsUnsignedLong(args);
    return PyLong_FromUnsignedLong(rd_fromle16(hostval));
}

PyObject* from_le32(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    u32 hostval = PyLong_AsUnsignedLong(args);
    return PyLong_FromUnsignedLong(rd_fromle32(hostval));
}

PyObject* from_le64(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    u64 hostval = PyLong_AsUnsignedLongLong(args);
    return PyLong_FromUnsignedLongLong(rd_fromle64(hostval));
}

PyObject* from_be16(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    u16 hostval = PyLong_AsUnsignedLong(args);
    return PyLong_FromUnsignedLong(rd_frombe16(hostval));
}

PyObject* from_be32(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    u32 hostval = PyLong_AsUnsignedLong(args);
    return PyLong_FromUnsignedLong(rd_frombe32(hostval));
}

PyObject* from_be64(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    u64 hostval = PyLong_AsUnsignedLongLong(args);
    return PyLong_FromUnsignedLongLong(rd_frombe64(hostval));
}

PyObject* to_le16(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    u16 hostval = PyLong_AsUnsignedLongLong(args);
    return PyLong_FromUnsignedLongLong(rd_tole16(hostval));
}

PyObject* to_le32(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    u32 hostval = PyLong_AsUnsignedLong(args);
    return PyLong_FromUnsignedLong(rd_tole32(hostval));
}

PyObject* to_le64(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    u64 hostval = PyLong_AsUnsignedLong(args);
    return PyLong_FromUnsignedLong(rd_tole64(hostval));
}

PyObject* to_be16(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    u16 hostval = PyLong_AsUnsignedLongLong(args);
    return PyLong_FromUnsignedLongLong(rd_tobe16(hostval));
}

PyObject* to_be32(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    u32 hostval = PyLong_AsUnsignedLongLong(args);
    return PyLong_FromUnsignedLongLong(rd_tobe32(hostval));
}

PyObject* to_be64(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    u64 hostval = PyLong_AsUnsignedLongLong(args);
    return PyLong_FromUnsignedLongLong(rd_tobe64(hostval));
}

} // namespace python
