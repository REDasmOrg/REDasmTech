#include "buffer.h"
#include "common.h"

namespace python {

namespace {

// clang-format off
struct PyBuffer {
    PyObject_HEAD
    RDBuffer* buffer;
};
// clang-format on

/* *** *** *** BUFFER *** *** *** */
PyObject* buffer_read(PyBuffer* self, PyObject* args) {
    u64 idx = 0, maxn = 0;
    if(!PyArg_ParseTuple(args, "KK", &idx, &maxn)) return nullptr;

    std::vector<char> d(maxn);
    u64 n = rdbuffer_read(self->buffer, idx, d.data(), d.size());
    return PyBytes_FromStringAndSize(d.data(), n);
}

PyObject* buffer_readstruct(PyBuffer* self, PyObject* args) {
    u64 idx = 0;
    PyObject* obj = nullptr;
    if(!PyArg_ParseTuple(args, "KO", &idx, &obj)) return nullptr;

    std::vector<RDStructField> s;
    if(!python::tuple_to_struct(obj, s)) return nullptr;

    RDValue* v = rdbuffer_readstruct(self->buffer, idx, s.data());
    PyObject* res = Py_None;

    if(v) {
        res = python::to_object(v);
        rdvalue_destroy(v);
    }

    return res;
}

PyObject* buffer_getbool(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    bool v;
    if(rdbuffer_getbool(self->buffer, idx, &v)) return PyBool_FromLong(v);
    return Py_None;
}

PyObject* buffer_getchar(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    char v;
    if(rdbuffer_getchar(self->buffer, idx, &v))
        return PyUnicode_FromStringAndSize(&v, 1);

    return Py_None;
}

PyObject* buffer_getu8(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    u8 v;
    if(rdbuffer_getu8(self->buffer, idx, &v)) return PyLong_FromUnsignedLong(v);
    return Py_None;
}

PyObject* buffer_getu16(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    u16 v;
    if(rdbuffer_getu16(self->buffer, idx, &v))
        return PyLong_FromUnsignedLong(v);

    return Py_None;
}

PyObject* buffer_getu32(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    u32 v;
    if(rdbuffer_getu32(self->buffer, idx, &v))
        return PyLong_FromUnsignedLong(v);

    return Py_None;
}

PyObject* buffer_getu64(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    u64 v;
    if(rdbuffer_getu64(self->buffer, idx, &v))
        return PyLong_FromUnsignedLongLong(v);

    return Py_None;
}

PyObject* buffer_geti8(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    i8 v;
    if(rdbuffer_geti8(self->buffer, idx, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* buffer_geti16(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    i16 v;
    if(rdbuffer_geti16(self->buffer, idx, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* buffer_geti32(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    i32 v;
    if(rdbuffer_geti32(self->buffer, idx, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* buffer_geti64(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    i64 v;
    if(rdbuffer_geti64(self->buffer, idx, &v)) return PyLong_FromLongLong(v);
    return Py_None;
}

PyObject* buffer_getu16be(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    u16 v;
    if(rdbuffer_getu16be(self->buffer, idx, &v))
        return PyLong_FromUnsignedLong(v);

    return Py_None;
}

PyObject* buffer_getu32be(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    u32 v;
    if(rdbuffer_getu32be(self->buffer, idx, &v))
        return PyLong_FromUnsignedLong(v);

    return Py_None;
}

PyObject* buffer_getu64be(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    u64 v;
    if(rdbuffer_getu64be(self->buffer, idx, &v))
        return PyLong_FromUnsignedLongLong(v);

    return Py_None;
}

PyObject* buffer_geti16be(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    i16 v;
    if(rdbuffer_geti16be(self->buffer, idx, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* buffer_geti32be(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    i32 v;
    if(rdbuffer_geti32be(self->buffer, idx, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* buffer_geti64be(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    i64 v;
    if(rdbuffer_geti64be(self->buffer, idx, &v)) return PyLong_FromLongLong(v);
    return Py_None;
}

PyObject* buffer_getstrz(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    const char* v;
    if(rdbuffer_getstrz(self->buffer, idx, &v)) return PyUnicode_FromString(v);
    return Py_None;
}

PyObject* buffer_getstr(PyBuffer* self, PyObject* args) {
    usize idx{}, n{};
    if(!PyArg_ParseTuple(args, "KK", &idx, &n)) return nullptr;

    const char* v;
    if(rdbuffer_getstr(self->buffer, idx, n, &v))
        return PyUnicode_FromString(v);

    return Py_None;
}

PyObject* buffer_getwstrz(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    const char* v;
    if(rdbuffer_getstrz(self->buffer, idx, &v)) return PyUnicode_FromString(v);
    return Py_None;
}

PyObject* buffer_getwstr(PyBuffer* self, PyObject* args) {
    usize idx{}, n{};
    if(!PyArg_ParseTuple(args, "KK", &idx, &n)) return nullptr;

    const char* v;
    if(rdbuffer_getstr(self->buffer, idx, n, &v))
        return PyUnicode_FromString(v);

    return Py_None;
}

PyObject* buffer_gettype(PyBuffer* self, PyObject* args) {
    usize idx{};
    const char* tname = nullptr;
    if(!PyArg_ParseTuple(args, "Ks*", &idx, &tname)) return nullptr;

    RDValue* v = rdbuffer_gettypename(self->buffer, idx, tname);
    PyObject* res = Py_None;

    if(v) {
        res = python::to_object(v);
        rdvalue_destroy(v);
    }

    return res;
}

// clang-format off
PyMethodDef buffer_methods[] = {
    {"read", reinterpret_cast<PyCFunction>(python::buffer_read), METH_VARARGS, nullptr},
    {"read_struct", reinterpret_cast<PyCFunction>(python::buffer_readstruct), METH_VARARGS, nullptr},
    {"get_bool", reinterpret_cast<PyCFunction>(python::buffer_getbool), METH_O, nullptr},
    {"get_char", reinterpret_cast<PyCFunction>(python::buffer_getchar), METH_O, nullptr},
    {"get_u8", reinterpret_cast<PyCFunction>(python::buffer_getu8), METH_O, nullptr},
    {"get_u16", reinterpret_cast<PyCFunction>(python::buffer_getu16), METH_O, nullptr},
    {"get_u32", reinterpret_cast<PyCFunction>(python::buffer_getu32), METH_O, nullptr},
    {"get_u64", reinterpret_cast<PyCFunction>(python::buffer_getu64), METH_O, nullptr},
    {"get_i8", reinterpret_cast<PyCFunction>(python::buffer_geti8), METH_O, nullptr},
    {"get_i16", reinterpret_cast<PyCFunction>(python::buffer_geti16), METH_O, nullptr},
    {"get_i32", reinterpret_cast<PyCFunction>(python::buffer_geti32), METH_O, nullptr},
    {"get_i64", reinterpret_cast<PyCFunction>(python::buffer_geti64), METH_O, nullptr},
    {"get_u16be", reinterpret_cast<PyCFunction>(python::buffer_getu16be), METH_O, nullptr},
    {"get_u32be", reinterpret_cast<PyCFunction>(python::buffer_getu32be), METH_O, nullptr},
    {"get_u64be", reinterpret_cast<PyCFunction>(python::buffer_getu64be), METH_O, nullptr},
    {"get_i16be", reinterpret_cast<PyCFunction>(python::buffer_geti16be), METH_O, nullptr},
    {"get_i32be", reinterpret_cast<PyCFunction>(python::buffer_geti32be), METH_O, nullptr},
    {"get_i64be", reinterpret_cast<PyCFunction>(python::buffer_geti64be), METH_O, nullptr},
    {"get_strz", reinterpret_cast<PyCFunction>(python::buffer_getstrz), METH_O, nullptr},
    {"get_str", reinterpret_cast<PyCFunction>(python::buffer_getstr), METH_VARARGS, nullptr},
    {"get_wstrz", reinterpret_cast<PyCFunction>(python::buffer_getwstrz), METH_O, nullptr},
    {"get_wstr", reinterpret_cast<PyCFunction>(python::buffer_getwstr), METH_VARARGS, nullptr},
    {"get_type", reinterpret_cast<PyCFunction>(python::buffer_gettype), METH_VARARGS, nullptr},
    {},
};
// clang-format on

/* *** *** *** FILE *** *** *** */

PySequenceMethods file_sequence_methods = []() {
    PySequenceMethods seq{};

    seq.sq_length = [](PyObject* self) -> Py_ssize_t {
        const auto* file = reinterpret_cast<const PyBuffer*>(self);
        return file ? rdbuffer_getlength(file->buffer) : 0;
    };

    seq.sq_item = [](PyObject* self, Py_ssize_t idx) {
        const auto* file = reinterpret_cast<const PyBuffer*>(self);
        auto uidx = static_cast<usize>(idx);
        if(!file || uidx >= rdbuffer_getlength(file->buffer)) return Py_None;

        u8 b;
        if(rdbuffer_getu8(file->buffer, uidx, &b)) return PyLong_FromSize_t(b);
        return Py_None;
    };

    return seq;
}();

PySequenceMethods memory_sequence_methods = []() {
    PySequenceMethods seq{};

    seq.sq_length = [](PyObject* self) -> Py_ssize_t {
        const auto* mem = reinterpret_cast<const PyBuffer*>(self);
        return mem ? rdbuffer_getlength(mem->buffer) : 0;
    };

    seq.sq_item = [](PyObject* self, Py_ssize_t idx) {
        const auto* mem = reinterpret_cast<const PyBuffer*>(self);

        RDMByte b;
        if(!rdbuffer_getmbyte(mem->buffer, static_cast<usize>(idx), &b))
            return Py_None;

        PyObject* pb = python::new_simplenamespace();
        // clang-format off
        PyObject_SetAttrString(pb, "name", PyBool_FromLong(rdmbyte_hasflag(b,
        BF_NAME))); PyObject_SetAttrString(pb, "segment",
        PyBool_FromLong(rdmbyte_hasflag(b, BF_SEGMENT)));
        PyObject_SetAttrString(pb, "import",
        PyBool_FromLong(rdmbyte_hasflag(b, BF_IMPORT)));
        PyObject_SetAttrString(pb, "export",
        PyBool_FromLong(rdmbyte_hasflag(b, BF_EXPORT)));
        PyObject_SetAttrString(pb, "unknown",
        PyBool_FromLong(rdmbyte_isunknown(b))); PyObject_SetAttrString(pb,
        "data", PyBool_FromLong(rdmbyte_isdata(b)));
        PyObject_SetAttrString(pb, "code",
        PyBool_FromLong(rdmbyte_iscode(b)));
        // clang-format on

        u8 v;

        if(rdmbyte_getbyte(b, &v))
            PyObject_SetAttrString(pb, "byte", PyLong_FromSize_t(v));
        else
            PyObject_SetAttrString(pb, "byte", Py_None);

        return pb;
    };

    return seq;
}();

} // namespace

PyTypeObject buffer_type = []() {
    PyTypeObject t{PyVarObject_HEAD_INIT(nullptr, 0)};
    t.tp_name = "redasm.Buffer";
    t.tp_basicsize = sizeof(PyBuffer);
    t.tp_flags = Py_TPFLAGS_DISALLOW_INSTANTIATION;
    t.tp_methods = python::buffer_methods;

    return t;
}();

PyTypeObject file_type = []() {
    PyTypeObject t{PyVarObject_HEAD_INIT(nullptr, 0)};
    t.tp_base = &python::buffer_type;
    t.tp_name = "redasm.File";
    t.tp_basicsize = sizeof(PyBuffer);
    t.tp_flags = Py_TPFLAGS_DEFAULT;
    t.tp_as_sequence = &python::file_sequence_methods;
    t.tp_new = PyType_GenericNew,

    t.tp_init = reinterpret_cast<initproc>(
        +[](PyBuffer* self, PyObject* args, PyObject* /*kwds*/) {
            const char* filepath = nullptr;
            if(!PyArg_ParseTuple(args, "z", &filepath)) return -1;
            self->buffer = rdbuffer_createfile(filepath);
            return 0;
        });

    t.tp_dealloc = reinterpret_cast<destructor>(+[](PyBuffer* self) {
        rdbuffer_destroy(self->buffer);
        Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
    });

    return t;
}();

PyTypeObject memory_type = []() {
    PyTypeObject t{PyVarObject_HEAD_INIT(nullptr, 0)};
    t.tp_base = &python::buffer_type;
    t.tp_name = "redasm.Memory";
    t.tp_basicsize = sizeof(PyBuffer);
    t.tp_flags = Py_TPFLAGS_DEFAULT;
    t.tp_as_sequence = &python::memory_sequence_methods;

    t.tp_init = reinterpret_cast<initproc>(
        +[](PyBuffer* self, PyObject* args, PyObject* /*kwds*/) {
            usize n = 0;
            if(!PyArg_ParseTuple(args, "n", &n)) return -1;
            self->buffer = rdbuffer_creatememory(n);
            return 0;
        });

    t.tp_dealloc = reinterpret_cast<destructor>(+[](PyBuffer* self) {
        rdbuffer_destroy(self->buffer);
        Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
    });

    return t;
}();

PyObject* pyfile_new() {
    PyBuffer* pyfile = PyObject_New(PyBuffer, &python::file_type);

    if(!pyfile) {
        PyErr_NoMemory();
        return nullptr;
    }

    pyfile->buffer = rd_getfile();
    return reinterpret_cast<PyObject*>(pyfile);
}

PyObject* pymemory_new() {
    PyBuffer* pymemory = PyObject_New(PyBuffer, &python::memory_type);

    if(!pymemory) {
        PyErr_NoMemory();
        return nullptr;
    }

    return reinterpret_cast<PyObject*>(pymemory);
}

PyObject* pybuffer_frombuffer(RDBuffer* b) {
    if(!b) return nullptr;

    PyBuffer* self = PyObject_New(PyBuffer, &python::buffer_type);

    if(!self) {
        PyErr_NoMemory();
        return nullptr;
    }

    self->buffer = b;
    return reinterpret_cast<PyObject*>(self);
}

RDBuffer* pybuffer_asbuffer(PyObject* obj) {
    if(obj) return reinterpret_cast<PyBuffer*>(obj)->buffer;
    return nullptr;
}

bool pybuffer_check(PyObject* obj) {
    return PyObject_TypeCheck(obj, &python::buffer_type);
}

} // namespace python
