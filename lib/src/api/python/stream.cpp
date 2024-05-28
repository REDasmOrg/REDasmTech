#include "stream.h"
#include "../../memory/memorystream.h"
#include "../../memory/stream.h"
#include "../internal/stream.h"
#include "../marshal.h"
#include "common.h"

namespace redasm::api::python {

namespace {

// clang-format off
struct PyAbstractStream {
    PyObject_HEAD
    RDStream* stream;
};

struct PyFileStream {
    PyAbstractStream super;
};

struct PyMemoryStream {
    PyAbstractStream super;
};
// clang-format on

PyObject* abstractstream_seek(PyAbstractStream* self, PyObject* args) {
    usize o = PyLong_AsUnsignedLongLong(args);
    return PyLong_FromUnsignedLongLong(internal::stream_seek(self->stream, o));
}

PyObject* abstractstream_move(PyAbstractStream* self, PyObject* args) {
    usize o = PyLong_AsUnsignedLongLong(args);
    return PyLong_FromUnsignedLongLong(internal::stream_move(self->stream, o));
}

PyObject* abstractstream_getposition(PyAbstractStream* self,
                                     PyObject* /*args*/) {
    return PyLong_FromUnsignedLongLong(
        internal::stream_getposition(self->stream));
}

PyObject* abstractstream_rewind(PyAbstractStream* self, PyObject* /*args*/) {
    internal::stream_rewind(self->stream);
    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_type(PyAbstractStream* self, PyObject* args) {
    const char* tname = PyUnicode_AsUTF8(args);

    if(auto v = internal::stream_peek_type(self->stream, tname); v)
        python::to_object(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_stringz(PyAbstractStream* self,
                                      PyObject* /*args*/) {
    if(auto v = internal::stream_peek_stringz(self->stream); v)
        return PyUnicode_FromString(v->c_str());

    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_string(PyAbstractStream* self, PyObject* args) {
    usize n = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::stream_peek_string(self->stream, n); v)
        return PyUnicode_FromString(v->c_str());

    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_u8(PyAbstractStream* self, PyObject* /*args*/) {
    if(auto v = internal::stream_peek_u8(self->stream); v)
        return PyLong_FromUnsignedLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_u16(PyAbstractStream* self, PyObject* /*args*/) {
    if(auto v = internal::stream_peek_u16(self->stream); v)
        return PyLong_FromUnsignedLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_u32(PyAbstractStream* self, PyObject* /*args*/) {
    if(auto v = internal::stream_peek_u32(self->stream); v)
        return PyLong_FromUnsignedLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_u64(PyAbstractStream* self, PyObject* /*args*/) {
    if(auto v = internal::stream_peek_u64(self->stream); v)
        return PyLong_FromUnsignedLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_i8(PyAbstractStream* self, PyObject* /*args*/) {
    if(auto v = internal::stream_peek_i8(self->stream); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_i16(PyAbstractStream* self, PyObject* /*args*/) {
    if(auto v = internal::stream_peek_i16(self->stream); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_i32(PyAbstractStream* self, PyObject* /*args*/) {
    if(auto v = internal::stream_peek_i32(self->stream); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_i64(PyAbstractStream* self, PyObject* /*args*/) {
    if(auto v = internal::stream_peek_i64(self->stream); v)
        return PyLong_FromLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_u16be(PyAbstractStream* self,
                                    PyObject* /*args*/) {
    if(auto v = internal::stream_peek_u16be(self->stream); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_u32be(PyAbstractStream* self,
                                    PyObject* /*args*/) {
    if(auto v = internal::stream_peek_u32be(self->stream); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_u64be(PyAbstractStream* self,
                                    PyObject* /*args*/) {
    if(auto v = internal::stream_peek_u64be(self->stream); v)
        return PyLong_FromLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_i16be(PyAbstractStream* self,
                                    PyObject* /*args*/) {
    if(auto v = internal::stream_peek_i16be(self->stream); v)
        return PyLong_FromLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_i32be(PyAbstractStream* self,
                                    PyObject* /*args*/) {
    if(auto v = internal::stream_peek_i32be(self->stream); v)
        return PyLong_FromLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_peek_i64be(PyAbstractStream* self,
                                    PyObject* /*args*/) {
    if(auto v = internal::stream_peek_i64be(self->stream); v)
        return PyLong_FromLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_type(PyAbstractStream* self, PyObject* args) {
    const char* tname = PyUnicode_AsUTF8(args);

    if(auto v = internal::stream_read_type(self->stream, tname); v)
        return python::to_object(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_stringz(PyAbstractStream* self,
                                      PyObject* /*args*/) {
    if(auto v = internal::stream_read_stringz(self->stream); v)
        return PyUnicode_FromString(v->c_str());

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_string(PyAbstractStream* self, PyObject* args) {
    usize n = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::stream_read_string(self->stream, n); v)
        return PyUnicode_FromString(v->c_str());

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_u8(PyAbstractStream* self, PyObject* /*args*/) {
    if(auto v = internal::stream_read_u8(self->stream); v)
        return PyLong_FromUnsignedLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_u16(PyAbstractStream* self, PyObject* /*args*/) {
    if(auto v = internal::stream_read_u16(self->stream); v)
        return PyLong_FromUnsignedLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_u32(PyAbstractStream* self, PyObject* /*args*/) {
    if(auto v = internal::stream_read_u32(self->stream); v)
        return PyLong_FromUnsignedLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_u64(PyAbstractStream* self, PyObject* /*args*/) {
    if(auto v = internal::stream_read_u64(self->stream); v)
        return PyLong_FromUnsignedLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_i8(PyAbstractStream* self, PyObject* /*args*/) {
    if(auto v = internal::stream_read_i8(self->stream); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_i16(PyAbstractStream* self, PyObject* /*args*/) {
    if(auto v = internal::stream_read_i16(self->stream); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_i32(PyAbstractStream* self, PyObject* /*args*/) {
    if(auto v = internal::stream_read_i32(self->stream); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_i64(PyAbstractStream* self, PyObject* /*args*/) {
    if(auto v = internal::stream_read_i64(self->stream); v)
        return PyLong_FromLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_u16be(PyAbstractStream* self,
                                    PyObject* /*args*/) {
    if(auto v = internal::stream_read_u16be(self->stream); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_u32be(PyAbstractStream* self,
                                    PyObject* /*args*/) {
    if(auto v = internal::stream_read_u32be(self->stream); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_u64be(PyAbstractStream* self,
                                    PyObject* /*args*/) {
    if(auto v = internal::stream_read_u64be(self->stream); v)
        return PyLong_FromLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_i16be(PyAbstractStream* self,
                                    PyObject* /*args*/) {
    if(auto v = internal::stream_read_i16be(self->stream); v)
        return PyLong_FromLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_i32be(PyAbstractStream* self,
                                    PyObject* /*args*/) {
    if(auto v = internal::stream_read_i32be(self->stream); v)
        return PyLong_FromLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_read_i64be(PyAbstractStream* self,
                                    PyObject* /*args*/) {
    if(auto v = internal::stream_read_i64be(self->stream); v)
        return PyLong_FromLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_collect_type(PyAbstractStream* self, PyObject* args) {
    const char* tname = PyUnicode_AsUTF8(args);

    if(auto v = internal::stream_collect_type(self->stream, tname); v)
        return python::to_object(*v);

    Py_RETURN_NONE;
}

PyObject* abstractstream_collect_stringz(PyAbstractStream* self,
                                         PyObject* /*args*/) {
    if(auto v = internal::stream_collect_stringz(self->stream); v)
        return PyUnicode_FromString(v->c_str());

    Py_RETURN_NONE;
}

PyObject* abstractstream_collect_string(PyAbstractStream* self,
                                        PyObject* args) {
    usize n = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::stream_collect_string(self->stream, n); v)
        return PyUnicode_FromString(v->c_str());

    Py_RETURN_NONE;
}

// clang-format off
PyMethodDef abstractstream_methods[] = {
    {"seek", reinterpret_cast<PyCFunction>(python::abstractstream_seek), METH_O, nullptr},
    {"move", reinterpret_cast<PyCFunction>(python::abstractstream_move), METH_O, nullptr},
    {"getposition", reinterpret_cast<PyCFunction>(python::abstractstream_getposition), METH_NOARGS, nullptr},
    {"rewind", reinterpret_cast<PyCFunction>(python::abstractstream_rewind), METH_NOARGS, nullptr},
    {"peek_type", reinterpret_cast<PyCFunction>(python::abstractstream_peek_type), METH_O, nullptr},
    {"peek_stringz", reinterpret_cast<PyCFunction>(python::abstractstream_peek_stringz), METH_NOARGS, nullptr},
    {"peek_string", reinterpret_cast<PyCFunction>(python::abstractstream_peek_string), METH_O, nullptr},
    {"peek_u8", reinterpret_cast<PyCFunction>(python::abstractstream_peek_u8), METH_NOARGS, nullptr},
    {"peek_u16", reinterpret_cast<PyCFunction>(python::abstractstream_peek_u16), METH_NOARGS, nullptr},
    {"peek_u32", reinterpret_cast<PyCFunction>(python::abstractstream_peek_u32), METH_NOARGS, nullptr},
    {"peek_u64", reinterpret_cast<PyCFunction>(python::abstractstream_peek_u64), METH_NOARGS, nullptr},
    {"peek_i8", reinterpret_cast<PyCFunction>(python::abstractstream_peek_i8), METH_NOARGS, nullptr},
    {"peek_i16", reinterpret_cast<PyCFunction>(python::abstractstream_peek_i16), METH_NOARGS, nullptr},
    {"peek_i32", reinterpret_cast<PyCFunction>(python::abstractstream_peek_i32), METH_NOARGS, nullptr},
    {"peek_i64", reinterpret_cast<PyCFunction>(python::abstractstream_peek_i64), METH_NOARGS, nullptr},
    {"peek_u16be", reinterpret_cast<PyCFunction>(python::abstractstream_peek_u16be), METH_NOARGS, nullptr},
    {"peek_u32be", reinterpret_cast<PyCFunction>(python::abstractstream_peek_u32be), METH_NOARGS, nullptr},
    {"peek_u64be", reinterpret_cast<PyCFunction>(python::abstractstream_peek_u64be), METH_NOARGS, nullptr},
    {"peek_i16be", reinterpret_cast<PyCFunction>(python::abstractstream_peek_i16be), METH_NOARGS, nullptr},
    {"peek_i32be", reinterpret_cast<PyCFunction>(python::abstractstream_peek_i32be), METH_NOARGS, nullptr},
    {"peek_i64be", reinterpret_cast<PyCFunction>(python::abstractstream_peek_i64be), METH_NOARGS, nullptr},
    {"read_type", reinterpret_cast<PyCFunction>(python::abstractstream_read_type), METH_O, nullptr},
    {"read_stringz", reinterpret_cast<PyCFunction>(python::abstractstream_read_stringz), METH_NOARGS, nullptr},
    {"read_string", reinterpret_cast<PyCFunction>(python::abstractstream_read_string), METH_O, nullptr},
    {"read_u8", reinterpret_cast<PyCFunction>(python::abstractstream_read_u8), METH_NOARGS, nullptr},
    {"read_u16", reinterpret_cast<PyCFunction>(python::abstractstream_read_u16), METH_NOARGS, nullptr},
    {"read_u32", reinterpret_cast<PyCFunction>(python::abstractstream_read_u32), METH_NOARGS, nullptr},
    {"read_u64", reinterpret_cast<PyCFunction>(python::abstractstream_read_u64), METH_NOARGS, nullptr},
    {"read_i8", reinterpret_cast<PyCFunction>(python::abstractstream_read_i8), METH_NOARGS, nullptr},
    {"read_i16", reinterpret_cast<PyCFunction>(python::abstractstream_read_i16), METH_NOARGS, nullptr},
    {"read_i32", reinterpret_cast<PyCFunction>(python::abstractstream_read_i32), METH_NOARGS, nullptr},
    {"read_i64", reinterpret_cast<PyCFunction>(python::abstractstream_read_i64), METH_NOARGS, nullptr},
    {"read_u16be", reinterpret_cast<PyCFunction>(python::abstractstream_read_u16be), METH_NOARGS, nullptr},
    {"read_u32be", reinterpret_cast<PyCFunction>(python::abstractstream_read_u32be), METH_NOARGS, nullptr},
    {"read_u64be", reinterpret_cast<PyCFunction>(python::abstractstream_read_u64be), METH_NOARGS, nullptr},
    {"read_i16be", reinterpret_cast<PyCFunction>(python::abstractstream_read_i16be), METH_NOARGS, nullptr},
    {"read_i32be", reinterpret_cast<PyCFunction>(python::abstractstream_read_i32be), METH_NOARGS, nullptr},
    {"read_i64be", reinterpret_cast<PyCFunction>(python::abstractstream_read_i64be), METH_NOARGS, nullptr},
    {"collect_type", reinterpret_cast<PyCFunction>(python::abstractstream_collect_type), METH_O, nullptr},
    {"collect_stringz", reinterpret_cast<PyCFunction>(python::abstractstream_collect_stringz), METH_NOARGS, nullptr},
    {"collect_string", reinterpret_cast<PyCFunction>(python::abstractstream_collect_string), METH_O, nullptr},
    {nullptr},
}; // clang-format on

} // namespace

PyTypeObject abstractstream_type = []() {
    PyTypeObject t{PyVarObject_HEAD_INIT(nullptr, 0)};
    t.tp_name = "redasm.AbstractStream";
    t.tp_basicsize = sizeof(PyAbstractStream);
    t.tp_flags = Py_TPFLAGS_DISALLOW_INSTANTIATION;
    t.tp_methods = python::abstractstream_methods;

    t.tp_dealloc = reinterpret_cast<destructor>(+[](PyAbstractStream* self) {
        delete api::from_c(self->stream);
        Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
    });

    return t;
}();

PyTypeObject filestream_type = []() {
    PyTypeObject t{PyVarObject_HEAD_INIT(nullptr, 0)};
    t.tp_base = &python::abstractstream_type;
    t.tp_name = "redasm.FileStream";
    t.tp_basicsize = sizeof(PyFileStream);
    t.tp_new = PyType_GenericNew;

    t.tp_init = reinterpret_cast<initproc>(
        +[](PyFileStream* self, PyObject* /*args*/, PyObject* /*kwds*/) {
            self->super.stream = api::to_c(new Stream());
            return 0;
        });

    return t;
}();

PyTypeObject memorystream_type = []() {
    PyTypeObject t{PyVarObject_HEAD_INIT(nullptr, 0)};
    t.tp_base = &python::abstractstream_type;
    t.tp_name = "redasm.MemoryStream";
    t.tp_basicsize = sizeof(PyFileStream);
    t.tp_new = PyType_GenericNew;

    t.tp_init = reinterpret_cast<initproc>(
        +[](PyMemoryStream* self, PyObject* /*args*/, PyObject* /*kwds*/) {
            self->super.stream = api::to_c(new MemoryStream());
            return 0;
        });

    return t;
}();

PyObject* pyfilestream_new() {
    return reinterpret_cast<PyObject*>(
        PyObject_New(PyFileStream, &python::filestream_type));
}

PyObject* pymemorystream_new() {
    return reinterpret_cast<PyObject*>(
        PyObject_New(PyMemoryStream, &python::memorystream_type));
}

} // namespace redasm::api::python
