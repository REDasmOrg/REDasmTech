#include "stream.h"
#include "buffer.h"
#include "common.h"
#include <redasm/redasm.h>

namespace python {

namespace {

// clang-format off
struct PyStream {
    PyObject_HEAD
    RDStream* stream;
};
// clang-format on

PyObject* stream_seek(PyStream* self, PyObject* args) {
    size_t o = PyLong_AsSize_t(args);
    return PyLong_FromUnsignedLongLong(rdstream_seek(self->stream, o));
}

PyObject* stream_move(PyStream* self, PyObject* args) {
    Py_ssize_t o = PyLong_AsSsize_t(args);
    return PyLong_FromUnsignedLongLong(rdstream_move(self->stream, o));
}

PyObject* stream_getpos(PyStream* self, PyObject* /*args*/) {
    return PyLong_FromSize_t(rdstream_getpos(self->stream));
}

PyObject* stream_rewind(PyStream* self, PyObject* /*args*/) {
    rdstream_rewind(self->stream);
    return Py_None;
}

PyObject* stream_peek_struct_n(PyStream* self, PyObject* args) {
    size_t idx = 0, n = 0;
    PyObject* fields = nullptr;
    if(!PyArg_ParseTuple(args, "nnO", &idx, &n, &fields)) return nullptr;

    std::vector<RDStructFieldDecl> s;
    if(!python::tuple_to_struct(fields, s)) return nullptr;

    RDValueOpt v = rdstream_peek_struct_n(self->stream, n, s.data());
    PyObject* obj = Py_None;

    if(v.ok) {
        obj = python::to_object(&v.value);
        rdvalue_destroy(&v.value);
    }

    return obj;
}

PyObject* stream_peek_struct(PyStream* self, PyObject* args) {
    std::vector<RDStructFieldDecl> s;
    if(!python::tuple_to_struct(args, s)) return nullptr;

    RDValueOpt v = rdstream_peek_struct(self->stream, s.data());
    PyObject* obj = Py_None;

    if(v.ok) {
        obj = python::to_object(&v.value);
        rdvalue_destroy(&v.value);
    }

    return obj;
}

PyObject* stream_peek_type(PyStream* self, PyObject* args) {
    const char* tname = PyUnicode_AsUTF8(args);

    RDValueOpt v = rdstream_peek_type(self->stream, tname);
    PyObject* obj = Py_None;

    if(v.ok) {
        obj = python::to_object(&v.value);
        rdvalue_destroy(&v.value);
    }

    return obj;
}

PyObject* stream_peek_strz(PyStream* self, PyObject* /*args*/) {
    const char* v;
    if(rdstream_peek_strz(self->stream, &v)) return PyUnicode_FromString(v);
    return Py_None;
}

PyObject* stream_peek_str(PyStream* self, PyObject* args) {
    usize n = PyLong_AsUnsignedLongLong(args);

    const char* v;
    if(rdstream_peek_str(self->stream, n, &v)) return PyUnicode_FromString(v);
    return Py_None;
}

PyObject* stream_peek_wstrz(PyStream* self, PyObject* /*args*/) {
    const char* v;
    if(rdstream_peek_wstrz(self->stream, &v)) return PyUnicode_FromString(v);
    return Py_None;
}

PyObject* stream_peek_wstr(PyStream* self, PyObject* args) {
    usize n = PyLong_AsUnsignedLongLong(args);

    const char* v;
    if(rdstream_peek_wstr(self->stream, n, &v)) return PyUnicode_FromString(v);
    return Py_None;
}

PyObject* stream_peek_u8(PyStream* self, PyObject* /*args*/) {
    u8 v;
    if(rdstream_peek_u8(self->stream, &v)) return PyLong_FromUnsignedLong(v);
    return Py_None;
}

PyObject* stream_peek_u16(PyStream* self, PyObject* /*args*/) {
    u16 v;
    if(rdstream_peek_u16(self->stream, &v)) return PyLong_FromUnsignedLong(v);
    return Py_None;
}

PyObject* stream_peek_u32(PyStream* self, PyObject* /*args*/) {
    u32 v;
    if(rdstream_peek_u32(self->stream, &v)) return PyLong_FromUnsignedLong(v);
    return Py_None;
}

PyObject* stream_peek_u64(PyStream* self, PyObject* /*args*/) {
    u64 v;
    if(rdstream_peek_u64(self->stream, &v))
        return PyLong_FromUnsignedLongLong(v);

    return Py_None;
}

PyObject* stream_peek_i8(PyStream* self, PyObject* /*args*/) {
    i8 v;
    if(rdstream_peek_i8(self->stream, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* stream_peek_i16(PyStream* self, PyObject* /*args*/) {
    i16 v;
    if(rdstream_peek_i16(self->stream, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* stream_peek_i32(PyStream* self, PyObject* /*args*/) {
    i32 v;
    if(rdstream_peek_i32(self->stream, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* stream_peek_i64(PyStream* self, PyObject* /*args*/) {
    i64 v;
    if(rdstream_peek_i64(self->stream, &v)) return PyLong_FromLongLong(v);
    return Py_None;
}

PyObject* stream_peek_u16be(PyStream* self, PyObject* /*args*/) {
    u16 v;
    if(rdstream_peek_u16be(self->stream, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* stream_peek_u32be(PyStream* self, PyObject* /*args*/) {
    u32 v;
    if(rdstream_peek_u32be(self->stream, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* stream_peek_u64be(PyStream* self, PyObject* /*args*/) {
    u64 v;
    if(rdstream_peek_u64be(self->stream, &v)) return PyLong_FromLongLong(v);
    return Py_None;
}

PyObject* stream_peek_i16be(PyStream* self, PyObject* /*args*/) {
    i16 v;
    if(rdstream_peek_i16be(self->stream, &v)) return PyLong_FromLongLong(v);
    return Py_None;
}

PyObject* stream_peek_i32be(PyStream* self, PyObject* /*args*/) {
    i32 v;
    if(rdstream_peek_i32be(self->stream, &v)) return PyLong_FromLongLong(v);
    return Py_None;
}

PyObject* stream_peek_i64be(PyStream* self, PyObject* /*args*/) {
    i64 v;
    if(rdstream_peek_i64be(self->stream, &v)) return PyLong_FromLongLong(v);
    return Py_None;
}

PyObject* stream_read_struct_n(PyStream* self, PyObject* args) {
    size_t idx = 0, n = 0;
    PyObject* fields = nullptr;
    if(!PyArg_ParseTuple(args, "nnO", &idx, &n, &fields)) return nullptr;

    std::vector<RDStructFieldDecl> s;
    if(!python::tuple_to_struct(fields, s)) return nullptr;

    RDValueOpt v = rdstream_read_struct_n(self->stream, n, s.data());
    PyObject* obj = Py_None;

    if(v.ok) {
        obj = python::to_object(&v.value);
        rdvalue_destroy(&v.value);
    }

    return obj;
}

PyObject* stream_read_struct(PyStream* self, PyObject* args) {
    std::vector<RDStructFieldDecl> s;
    if(!python::tuple_to_struct(args, s)) return nullptr;

    RDValueOpt v = rdstream_read_struct(self->stream, s.data());
    PyObject* obj = Py_None;

    if(v.ok) {
        obj = python::to_object(&v.value);
        rdvalue_destroy(&v.value);
    }

    return obj;
}

PyObject* stream_read_type(PyStream* self, PyObject* args) {
    const char* tname = PyUnicode_AsUTF8(args);
    RDValueOpt v = rdstream_read_type(self->stream, tname);
    PyObject* res = Py_None;

    if(v.ok) {
        res = python::to_object(&v.value);
        rdvalue_destroy(&v.value);
    }

    return res;
}

PyObject* stream_read_strz(PyStream* self, PyObject* /*args*/) {
    const char* v;
    if(rdstream_read_strz(self->stream, &v)) return PyUnicode_FromString(v);
    return Py_None;
}

PyObject* stream_read_str(PyStream* self, PyObject* args) {
    usize n = PyLong_AsUnsignedLongLong(args);
    const char* v;

    if(rdstream_read_str(self->stream, n, &v)) return PyUnicode_FromString(v);
    return Py_None;
}

PyObject* stream_read_wstrz(PyStream* self, PyObject* /*args*/) {
    const char* v;
    if(rdstream_read_wstrz(self->stream, &v)) return PyUnicode_FromString(v);
    return Py_None;
}

PyObject* stream_read_wstr(PyStream* self, PyObject* args) {
    usize n = PyLong_AsUnsignedLongLong(args);

    const char* v;
    if(rdstream_read_wstr(self->stream, n, &v)) return PyUnicode_FromString(v);
    return Py_None;
}

PyObject* stream_read_u8(PyStream* self, PyObject* /*args*/) {
    u8 v;
    if(rdstream_read_u8(self->stream, &v)) return PyLong_FromUnsignedLong(v);
    return Py_None;
}

PyObject* stream_read_u16(PyStream* self, PyObject* /*args*/) {
    u16 v;
    if(rdstream_read_u16(self->stream, &v)) return PyLong_FromUnsignedLong(v);
    return Py_None;
}

PyObject* stream_read_u32(PyStream* self, PyObject* /*args*/) {
    u32 v;
    if(rdstream_read_u32(self->stream, &v)) return PyLong_FromUnsignedLong(v);
    return Py_None;
}

PyObject* stream_read_u64(PyStream* self, PyObject* /*args*/) {
    u64 v;
    if(rdstream_read_u64(self->stream, &v))
        return PyLong_FromUnsignedLongLong(v);

    return Py_None;
}

PyObject* stream_read_i8(PyStream* self, PyObject* /*args*/) {
    i8 v;
    if(rdstream_read_i8(self->stream, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* stream_read_i16(PyStream* self, PyObject* /*args*/) {
    i16 v;
    if(rdstream_read_i16(self->stream, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* stream_read_i32(PyStream* self, PyObject* /*args*/) {
    i32 v;
    if(rdstream_read_i32(self->stream, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* stream_read_i64(PyStream* self, PyObject* /*args*/) {
    i64 v;
    if(rdstream_read_i64(self->stream, &v)) return PyLong_FromLongLong(v);
    return Py_None;
}

PyObject* stream_read_u16be(PyStream* self, PyObject* /*args*/) {
    u16 v;
    if(rdstream_read_u16be(self->stream, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* stream_read_u32be(PyStream* self, PyObject* /*args*/) {
    u32 v;
    if(rdstream_read_u32be(self->stream, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* stream_read_u64be(PyStream* self, PyObject* /*args*/) {
    u64 v;
    if(rdstream_read_u64be(self->stream, &v)) return PyLong_FromLongLong(v);
    return Py_None;
}

PyObject* stream_read_i16be(PyStream* self, PyObject* /*args*/) {
    i16 v;
    if(rdstream_read_i16be(self->stream, &v)) return PyLong_FromLongLong(v);
    return Py_None;
}

PyObject* stream_read_i32be(PyStream* self, PyObject* /*args*/) {
    i32 v;
    if(rdstream_read_i32be(self->stream, &v)) return PyLong_FromLongLong(v);
    return Py_None;
}

PyObject* stream_read_i64be(PyStream* self, PyObject* /*args*/) {
    i64 v;
    if(rdstream_read_i64be(self->stream, &v)) return PyLong_FromLongLong(v);
    return Py_None;
}

// clang-format off
PyMethodDef stream_methods[] = {
    {"seek", reinterpret_cast<PyCFunction>(python::stream_seek), METH_O, nullptr},
    {"move", reinterpret_cast<PyCFunction>(python::stream_move), METH_O, nullptr},
    {"get_pos", reinterpret_cast<PyCFunction>(python::stream_getpos), METH_NOARGS, nullptr},
    {"rewind", reinterpret_cast<PyCFunction>(python::stream_rewind), METH_NOARGS, nullptr},
    {"peek_struct_n", reinterpret_cast<PyCFunction>(python::stream_peek_struct_n), METH_VARARGS, nullptr},
    {"peek_struct", reinterpret_cast<PyCFunction>(python::stream_peek_struct), METH_O, nullptr},
    {"peek_type", reinterpret_cast<PyCFunction>(python::stream_peek_type), METH_O, nullptr},
    {"peek_strz", reinterpret_cast<PyCFunction>(python::stream_peek_strz), METH_NOARGS, nullptr},
    {"peek_str", reinterpret_cast<PyCFunction>(python::stream_peek_str), METH_O, nullptr},
    {"peek_wstrz", reinterpret_cast<PyCFunction>(python::stream_peek_wstrz), METH_NOARGS, nullptr},
    {"peek_wstr", reinterpret_cast<PyCFunction>(python::stream_peek_wstr), METH_O, nullptr},
    {"peek_u8", reinterpret_cast<PyCFunction>(python::stream_peek_u8), METH_NOARGS, nullptr},
    {"peek_u16", reinterpret_cast<PyCFunction>(python::stream_peek_u16), METH_NOARGS, nullptr},
    {"peek_u32", reinterpret_cast<PyCFunction>(python::stream_peek_u32), METH_NOARGS, nullptr},
    {"peek_u64", reinterpret_cast<PyCFunction>(python::stream_peek_u64), METH_NOARGS, nullptr},
    {"peek_i8", reinterpret_cast<PyCFunction>(python::stream_peek_i8), METH_NOARGS, nullptr},
    {"peek_i16", reinterpret_cast<PyCFunction>(python::stream_peek_i16), METH_NOARGS, nullptr},
    {"peek_i32", reinterpret_cast<PyCFunction>(python::stream_peek_i32), METH_NOARGS, nullptr},
    {"peek_i64", reinterpret_cast<PyCFunction>(python::stream_peek_i64), METH_NOARGS, nullptr},
    {"peek_u16be", reinterpret_cast<PyCFunction>(python::stream_peek_u16be), METH_NOARGS, nullptr},
    {"peek_u32be", reinterpret_cast<PyCFunction>(python::stream_peek_u32be), METH_NOARGS, nullptr},
    {"peek_u64be", reinterpret_cast<PyCFunction>(python::stream_peek_u64be), METH_NOARGS, nullptr},
    {"peek_i16be", reinterpret_cast<PyCFunction>(python::stream_peek_i16be), METH_NOARGS, nullptr},
    {"peek_i32be", reinterpret_cast<PyCFunction>(python::stream_peek_i32be), METH_NOARGS, nullptr},
    {"peek_i64be", reinterpret_cast<PyCFunction>(python::stream_peek_i64be), METH_NOARGS, nullptr},
    {"read_struct_n", reinterpret_cast<PyCFunction>(python::stream_read_struct_n), METH_VARARGS, nullptr},
    {"read_struct", reinterpret_cast<PyCFunction>(python::stream_read_struct), METH_O, nullptr},
    {"read_type", reinterpret_cast<PyCFunction>(python::stream_read_type), METH_O, nullptr},
    {"read_strz", reinterpret_cast<PyCFunction>(python::stream_read_strz), METH_NOARGS, nullptr},
    {"read_str", reinterpret_cast<PyCFunction>(python::stream_read_str), METH_O, nullptr},
    {"read_wstrz", reinterpret_cast<PyCFunction>(python::stream_read_wstrz), METH_NOARGS, nullptr},
    {"read_wstr", reinterpret_cast<PyCFunction>(python::stream_read_wstr), METH_O, nullptr},
    {"read_u8", reinterpret_cast<PyCFunction>(python::stream_read_u8), METH_NOARGS, nullptr},
    {"read_u16", reinterpret_cast<PyCFunction>(python::stream_read_u16), METH_NOARGS, nullptr},
    {"read_u32", reinterpret_cast<PyCFunction>(python::stream_read_u32), METH_NOARGS, nullptr},
    {"read_u64", reinterpret_cast<PyCFunction>(python::stream_read_u64), METH_NOARGS, nullptr},
    {"read_i8", reinterpret_cast<PyCFunction>(python::stream_read_i8), METH_NOARGS, nullptr},
    {"read_i16", reinterpret_cast<PyCFunction>(python::stream_read_i16), METH_NOARGS, nullptr},
    {"read_i32", reinterpret_cast<PyCFunction>(python::stream_read_i32), METH_NOARGS, nullptr},
    {"read_i64", reinterpret_cast<PyCFunction>(python::stream_read_i64), METH_NOARGS, nullptr},
    {"read_u16be", reinterpret_cast<PyCFunction>(python::stream_read_u16be), METH_NOARGS, nullptr},
    {"read_u32be", reinterpret_cast<PyCFunction>(python::stream_read_u32be), METH_NOARGS, nullptr},
    {"read_u64be", reinterpret_cast<PyCFunction>(python::stream_read_u64be), METH_NOARGS, nullptr},
    {"read_i16be", reinterpret_cast<PyCFunction>(python::stream_read_i16be), METH_NOARGS, nullptr},
    {"read_i32be", reinterpret_cast<PyCFunction>(python::stream_read_i32be), METH_NOARGS, nullptr},
    {"read_i64be", reinterpret_cast<PyCFunction>(python::stream_read_i64be), METH_NOARGS, nullptr},
    {},
}; // clang-format on

} // namespace

PyTypeObject stream_type = []() {
    PyTypeObject t{PyVarObject_HEAD_INIT(nullptr, 0)};
    t.tp_name = "redasm.Stream";
    t.tp_basicsize = sizeof(PyStream);
    t.tp_flags = Py_TPFLAGS_DEFAULT;
    t.tp_methods = python::stream_methods;
    t.tp_new = PyType_GenericNew;

    t.tp_dealloc = reinterpret_cast<destructor>(+[](PyStream* self) {
        rdstream_destroy(self->stream);
        Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
    });

    t.tp_init = reinterpret_cast<initproc>(
        +[](PyStream* self, PyObject* args, PyObject* /*kwds*/) {
            PyObject* buffer = nullptr;
            if(!PyArg_ParseTuple(args, "O", &buffer)) return -1;

            if(!pybuffer_check(buffer)) {
                python::type_error(buffer, "redasm.Buffer");
                return -1;
            }

            self->stream = rdstream_create(pybuffer_asbuffer(buffer));
            return 0;
        });

    return t;
}();

PyObject* pystream_new() {
    return reinterpret_cast<PyObject*>(
        PyObject_New(PyStream, &python::stream_type));
}

} // namespace python
