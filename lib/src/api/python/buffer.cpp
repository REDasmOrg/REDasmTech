#include "buffer.h"
#include "../../context.h"
#include "../../state.h"
#include "../internal/buffer.h"
#include "../internal/file.h"
#include "../internal/memory.h"
#include "common.h"

namespace redasm::api::python {

namespace {

// clang-format off
struct PyBuffer {
    PyObject_HEAD
    RDBuffer* buffer;
};

struct PyFile {
    PyBuffer super;
};

struct PyMemory {
    PyBuffer super;
};
// clang-format on

/* *** *** *** BUFFER *** *** *** */
PyObject* buffer_read(PyBuffer* self, PyObject* args) {
    u64 idx = 0, maxn = 0;
    if(!PyArg_ParseTuple(args, "KK", &idx, &maxn)) return nullptr;

    std::vector<char> d(maxn);
    u64 n = internal::buffer_read(self->buffer, idx, d.data(), d.size());
    return PyBytes_FromStringAndSize(d.data(), n);
}

PyObject* buffer_readstruct(PyBuffer* self, PyObject* args) {
    u64 idx = 0;
    PyObject* obj = nullptr;
    if(!PyArg_ParseTuple(args, "KO", &idx, &obj)) return nullptr;

    typing::Struct s;
    if(!python::tuple_to_struct(obj, s)) return nullptr;

    if(auto v = internal::buffer_readstruct(self->buffer, idx, s); v)
        return python::to_object(*v);

    return Py_None;
}

PyObject* buffer_getbool(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_getbool(self->buffer, idx); v)
        return PyBool_FromLong(*v);

    return Py_None;
}

PyObject* buffer_getchar(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_getchar(self->buffer, idx); v)
        return PyUnicode_FromStringAndSize(&v.value(), 1);

    return Py_None;
}

PyObject* buffer_getu8(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_getu8(self->buffer, idx); v)
        return PyLong_FromUnsignedLong(*v);

    return Py_None;
}

PyObject* buffer_getu16(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_getu16(self->buffer, idx); v)
        return PyLong_FromUnsignedLong(*v);

    return Py_None;
}

PyObject* buffer_getu32(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_getu32(self->buffer, idx); v)
        return PyLong_FromUnsignedLong(*v);

    return Py_None;
}

PyObject* buffer_getu64(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_getu64(self->buffer, idx); v)
        return PyLong_FromUnsignedLongLong(*v);

    return Py_None;
}

PyObject* buffer_geti8(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_geti8(self->buffer, idx); v)
        return PyLong_FromLong(*v);

    return Py_None;
}

PyObject* buffer_geti16(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_geti16(self->buffer, idx); v)
        return PyLong_FromLong(*v);

    return Py_None;
}

PyObject* buffer_geti32(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_geti32(self->buffer, idx); v)
        return PyLong_FromLong(*v);

    return Py_None;
}

PyObject* buffer_geti64(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_geti64(self->buffer, idx); v)
        return PyLong_FromLongLong(*v);

    return Py_None;
}

PyObject* buffer_getu16be(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_getu16be(self->buffer, idx); v)
        return PyLong_FromUnsignedLong(*v);

    return Py_None;
}

PyObject* buffer_getu32be(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_getu32be(self->buffer, idx); v)
        return PyLong_FromUnsignedLong(*v);

    return Py_None;
}

PyObject* buffer_getu64be(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_getu64be(self->buffer, idx); v)
        return PyLong_FromUnsignedLongLong(*v);

    return Py_None;
}

PyObject* buffer_geti16be(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_geti16be(self->buffer, idx); v)
        return PyLong_FromLong(*v);

    return Py_None;
}

PyObject* buffer_geti32be(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_geti32be(self->buffer, idx); v)
        return PyLong_FromLong(*v);

    return Py_None;
}

PyObject* buffer_geti64be(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_geti64be(self->buffer, idx); v)
        return PyLong_FromLongLong(*v);

    return Py_None;
}

PyObject* buffer_getstrz(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_getstrz(self->buffer, idx); v)
        return PyUnicode_FromString(v->c_str());

    return Py_None;
}

PyObject* buffer_getstr(PyBuffer* self, PyObject* args) {
    usize idx{}, n{};
    if(!PyArg_ParseTuple(args, "KK", &idx, &n)) return nullptr;

    if(auto v = internal::buffer_getstr(self->buffer, idx, n); v)
        return PyUnicode_FromString(v->c_str());

    return Py_None;
}

PyObject* buffer_getwstrz(PyBuffer* self, PyObject* args) {
    if(!PyLong_Check(args)) python::type_error(args, "int");
    usize idx = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::buffer_getstrz(self->buffer, idx); v)
        return PyUnicode_FromString(v->c_str());

    return Py_None;
}

PyObject* buffer_getwstr(PyBuffer* self, PyObject* args) {
    usize idx{}, n{};
    if(!PyArg_ParseTuple(args, "KK", &idx, &n)) return nullptr;

    if(auto v = internal::buffer_getstr(self->buffer, idx, n); v)
        return PyUnicode_FromString(v->c_str());

    return Py_None;
}

PyObject* buffer_gettype(PyBuffer* self, PyObject* args) {
    usize idx{};
    const char* tname = nullptr;
    if(!PyArg_ParseTuple(args, "Ks*", &idx, &tname)) return nullptr;

    if(auto v = internal::buffer_gettypename(self->buffer, idx, tname); v)
        return python::to_object(*v);

    return Py_None;
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
    {nullptr},
};
// clang-format on

/* *** *** *** FILE *** *** *** */

PySequenceMethods file_sequence_methods = []() {
    PySequenceMethods seq{};

    seq.sq_length = [](PyObject*) -> Py_ssize_t {
        if(!state::context->file) return 0;

        return state::context->file->size();
    };

    seq.sq_item = [](PyObject*, Py_ssize_t idx) {
        auto uidx = static_cast<usize>(idx);

        if(!state::context->file || uidx >= state::context->file->size())
            return Py_None;

        auto b = state::context->file->get_byte(uidx);
        if(b) return PyLong_FromSize_t(*b);
        return Py_None;
    };

    return seq;
}();

PyObject* file_map_type(PyFile* /*self*/, PyObject* args) {
    RDOffset offset{};
    const char* tname;

    if(!PyArg_ParseTuple(args, "Kz", &offset, &tname)) return nullptr;

    if(auto v = internal::file_map_type(offset, tname); v)
        return python::to_object(*v);

    return Py_None;
}

PyObject* file_map_type_as(PyFile* /*self*/, PyObject* args) {
    RDOffset offset{};
    const char *tname, *dbname;

    if(!PyArg_ParseTuple(args, "Kzz", &offset, &tname, &dbname)) return nullptr;

    if(auto v = internal::file_map_type_as(offset, tname, dbname); v)
        return python::to_object(*v);

    return Py_None;
}

// clang-format off
PyMethodDef file_methods[] = {
    {"map_type", reinterpret_cast<PyCFunction>(python::file_map_type), METH_VARARGS, nullptr},
    {"map_type_as", reinterpret_cast<PyCFunction>(python::file_map_type_as), METH_VARARGS, nullptr},
    {nullptr},
};
// clang-format on

/* *** *** *** MEMORY *** *** *** */
PyObject* memory_map_n(PyMemory* /*self*/, PyObject* args) {
    usize base{}, size{};

    if(!PyArg_ParseTuple(args, "KK", &base, &size)) return nullptr;

    return PyBool_FromLong(internal::memory_map_n(base, size));
}

PyObject* memory_map(PyMemory* /*self*/, PyObject* args) {
    RDAddress startaddr{}, endaddr{};

    if(!PyArg_ParseTuple(args, "KK", &startaddr, &endaddr)) return nullptr;

    return PyBool_FromLong(internal::memory_map(startaddr, endaddr));
}

PyObject* memory_copy(PyMemory* /*self*/, PyObject* args) {
    RDAddress address{};
    RDOffset start{}, end{};

    if(!PyArg_ParseTuple(args, "KKK", &address, &start, &end)) return nullptr;

    internal::memory_copy(address, start, end);
    return Py_None;
}

PyObject* memory_copy_n(PyMemory* /*self*/, PyObject* args) {
    RDAddress address{}, start{};
    usize size{};

    if(!PyArg_ParseTuple(args, "KKK", &address, &start, &size)) return nullptr;

    internal::memory_copy_n(address, start, size);
    return Py_None;
}

PySequenceMethods memory_sequence_methods = []() {
    PySequenceMethods seq{};

    seq.sq_length = [](PyObject*) -> Py_ssize_t {
        if(!state::context->memory) return 0;

        return state::context->memory->size();
    };

    seq.sq_item = [](PyObject*, Py_ssize_t idx) {
        auto uidx = static_cast<usize>(idx);

        if(!state::context->memory || uidx >= state::context->memory->size())
            return Py_None;

        Byte b = state::context->memory->at(uidx);
        PyObject* pb = python::new_simplenamespace();

        // clang-format off
        PyObject_SetAttrString(pb, "name", PyBool_FromLong(b.has(BF_NAME)));
        PyObject_SetAttrString(pb, "segment", PyBool_FromLong(b.has(BF_SEGMENT)));
        PyObject_SetAttrString(pb, "import", PyBool_FromLong(b.has(BF_IMPORT)));
        PyObject_SetAttrString(pb, "export", PyBool_FromLong(b.has(BF_EXPORT)));
        PyObject_SetAttrString(pb, "unknown", PyBool_FromLong(b.is_unknown()));
        PyObject_SetAttrString(pb, "data", PyBool_FromLong(b.is_data()));
        PyObject_SetAttrString(pb, "code", PyBool_FromLong(b.is_code()));
        // clang-format on

        if(b.has_byte())
            PyObject_SetAttrString(pb, "byte", PyLong_FromSize_t(b.byte()));
        else
            PyObject_SetAttrString(pb, "byte", Py_None);

        return pb;
    };

    return seq;
}();

// clang-format off
PyMethodDef memory_methods[] = {
    {"map", reinterpret_cast<PyCFunction>(python::memory_map), METH_VARARGS, nullptr},
    {"map_n", reinterpret_cast<PyCFunction>(python::memory_map_n), METH_VARARGS, nullptr},
    {"copy", reinterpret_cast<PyCFunction>(python::memory_copy), METH_VARARGS, nullptr},
    {"copy_n", reinterpret_cast<PyCFunction>(python::memory_copy_n), METH_VARARGS, nullptr},
    {nullptr},
};
// clang-format on

PyTypeObject buffer_type = []() {
    PyTypeObject t{PyVarObject_HEAD_INIT(nullptr, 0)};
    t.tp_name = "redasm.Buffer";
    t.tp_basicsize = sizeof(PyBuffer);
    t.tp_flags = Py_TPFLAGS_DISALLOW_INSTANTIATION;
    t.tp_methods = python::buffer_methods;
    t.tp_as_sequence = &python::file_sequence_methods;

    return t;
}();

} // namespace

PyTypeObject file_type = []() {
    PyTypeObject t{PyVarObject_HEAD_INIT(nullptr, 0)};
    t.tp_base = &python::buffer_type;
    t.tp_name = "redasm.File";
    t.tp_basicsize = sizeof(PyFile);
    t.tp_flags = Py_TPFLAGS_DISALLOW_INSTANTIATION;
    t.tp_methods = python::file_methods;
    t.tp_as_sequence = &python::file_sequence_methods;
    return t;
}();

PyTypeObject memory_type = []() {
    PyTypeObject t{PyVarObject_HEAD_INIT(nullptr, 0)};
    t.tp_base = &python::buffer_type;
    t.tp_name = "redasm.Memory";
    t.tp_basicsize = sizeof(PyMemory);
    t.tp_flags = Py_TPFLAGS_DISALLOW_INSTANTIATION;
    t.tp_methods = python::memory_methods;
    t.tp_as_sequence = &python::memory_sequence_methods;
    return t;
}();

PyObject* pyfile_new() {
    PyFile* pyfile = PyObject_New(PyFile, &file_type);

    if(!pyfile) {
        PyErr_NoMemory();
        return nullptr;
    }

    pyfile->super.buffer = internal::buffer_getfile();
    return reinterpret_cast<PyObject*>(pyfile);
}

PyObject* pyfile_frombuffer(RDBuffer* buffer) {
    if(!buffer) return nullptr;

    PyFile* pyfile = PyObject_New(PyFile, &file_type);

    if(!pyfile) {
        PyErr_NoMemory();
        return nullptr;
    }

    pyfile->super.buffer = buffer;
    return reinterpret_cast<PyObject*>(pyfile);
}

RDBuffer* pyfile_asbuffer(PyObject* file) {
    if(file) return reinterpret_cast<PyFile*>(file)->super.buffer;
    return nullptr;
}

PyObject* pymemory_new() {
    PyMemory* pymemory = PyObject_New(PyMemory, &memory_type);

    if(!pymemory) {
        PyErr_NoMemory();
        return nullptr;
    }

    pymemory->super.buffer = internal::buffer_getmemory();
    return reinterpret_cast<PyObject*>(pymemory);
}

PyObject* pymemory_frombuffer(RDBuffer* buffer) {
    if(!buffer) return nullptr;

    PyMemory* pymemory = PyObject_New(PyMemory, &file_type);

    if(!pymemory) {
        PyErr_NoMemory();
        return nullptr;
    }

    return reinterpret_cast<PyObject*>(pymemory);
}

RDBuffer* pymemory_asbuffer(PyObject* memory) {
    if(memory) return reinterpret_cast<PyMemory*>(memory)->super.buffer;
    return nullptr;
}

} // namespace redasm::api::python
