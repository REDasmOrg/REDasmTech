#include "common.h"
#include "../../error.h"
#include "../../state.h"
#include "../../typing/base.h"
#include "buffer.h"

namespace redasm::api::python {

bool validate_class(PyObject* obj,
                    std::initializer_list<const char*> reqattrs) {
    if(!PyType_Check(obj)) return python::type_error(obj, "class");

    for(const char* attr : reqattrs) {
        if(!PyObject_HasAttrString(obj, attr))
            return python::attr_error(obj, attr);
    }

    return true;
}

bool tuple_to_struct(PyObject* obj, typing::Struct& s) {
    if(!PyTuple_Check(obj)) {
        PyErr_SetString(PyExc_TypeError, "Expected a tuple");
        return false;
    }

    Py_ssize_t size = PyTuple_Size(obj);
    s.clear();
    s.reserve(size);

    for(Py_ssize_t i = 0; i < size; ++i) {
        PyObject* field = PyTuple_GetItem(obj, i);

        if(!PyTuple_Check(field)) {
            PyErr_SetString(PyExc_TypeError, "Expected a tuple");
            return false;
        }

        if(auto s = PyTuple_Size(field); s != 2) {
            PyErr_SetString(PyExc_TypeError, "Invalid tuple size");
            return false;
        }

        const char* ftype = PyUnicode_AsUTF8(PyTuple_GetItem(field, 0));
        const char* fname = PyUnicode_AsUTF8(PyTuple_GetItem(field, 1));
        s.emplace_back(ftype, fname);
    }

    return true;
}

PyObject* new_simplenamespace() {
    PyObject* mod_types = PyImport_ImportModule("types");
    PyObject* ns = PyObject_GetAttrString(mod_types, "SimpleNamespace");
    PyObject* obj = PyObject_CallNoArgs(ns);

    Py_DECREF(ns);
    Py_DECREF(mod_types);
    return obj;
}

PyObject* to_object(const typing::Value& v) {
    PyObject* res = nullptr;

    if(v.is_list()) {
        assume(v.type.n == v.list.size());

        if(v.type.id == typing::ids::CHAR ||
           v.type.id == typing::ids::WCHAR) { // (w)char[N]: convert to string
            assume(v.type.n == v.list.size());
            res = PyUnicode_New(v.type.n, 127);

            for(usize i = 0; i < v.list.size(); i++) {
                if(v.list[i].ch_v) // Check for '\0'
                    assume(PyUnicode_WriteChar(res, i, v.list[i].ch_v) == 0);
                else {
                    PyUnicode_Resize(&res, i); // Resize to null-terminator
                    break;
                }
            }
        }
        else {
            res = PyList_New(v.list.size());

            for(usize i = 0; i < v.list.size(); i++)
                PyList_SetItem(res, i, python::to_object(v.list[i]));
        }
    }
    else if(v.is_struct()) {
        res = python::new_simplenamespace();

        for(const auto& [name, field] : v.dict) {
            PyObject* f = python::to_object(field);
            assume(f);
            PyObject_SetAttrString(res, name.data(), f);
            Py_DECREF(f);
        }
    }
    else {
        switch(v.type.id) {
            case typing::ids::BOOL: res = PyBool_FromLong(v.b_v); break;

            case typing::ids::WCHAR:
            case typing::ids::CHAR:
                res = PyUnicode_FromStringAndSize(&v.ch_v, 1);
                break;

            case typing::ids::U8: res = PyLong_FromUnsignedLong(v.u8_v); break;

            case typing::ids::U16:
                res = PyLong_FromUnsignedLong(v.u16_v);
                break;

            case typing::ids::U32:
                res = PyLong_FromUnsignedLong(v.u32_v);
                break;

            case typing::ids::U64:
                res = PyLong_FromUnsignedLongLong(v.u64_v);
                break;

            case typing::ids::I8: res = PyLong_FromLong(v.i8_v); break;
            case typing::ids::I16: res = PyLong_FromLong(v.i16_v); break;
            case typing::ids::I32: res = PyLong_FromLong(v.i32_v); break;
            case typing::ids::I64: res = PyLong_FromLongLong(v.i64_v); break;

            case typing::ids::STR:
            case typing::ids::WSTR:
                res = PyUnicode_FromStringAndSize(v.str.data(), v.str.size());
                break;

            default: unreachable;
        }
    }

    return res;
}

void attach_methods(PyObject* obj, PyMethodDef* methods) {
    for(PyMethodDef* md = methods; md->ml_name; md++) {
        PyObject* func = PyCFunction_New(md, nullptr);
        PyObject* meth = PyInstanceMethod_New(func);
        PyObject_SetAttrString(obj, md->ml_name, meth);
        Py_DECREF(func);
    }
}

void check_error() {
    if(!PyErr_Occurred()) return;

    std::string s;
    PyObject* exc = PyErr_GetRaisedException();

    if(exc) {
        PyObject* strexc = PyObject_Str(exc);
        s += PyUnicode_AsUTF8(strexc);
        Py_DECREF(strexc);
    }

    auto* strace =
        reinterpret_cast<PyTracebackObject*>(PyException_GetTraceback(exc));

    if(strace) {
        PyTracebackObject* t = strace;

        if(!s.empty()) s += '\n';

        s += "Traceback (most recent call last):";

        while(t) {
            PyFrameObject* frame = t->tb_frame;

            while(frame) {
                PyCodeObject* fcode = PyFrame_GetCode(frame);
                s += fmt::format("\n File \"{}\", line {}, in {}",
                                 PyUnicode_AsUTF8(fcode->co_filename),
                                 PyFrame_GetLineNumber(frame),
                                 PyUnicode_AsUTF8(fcode->co_name));

                frame = PyFrame_GetBack(frame);
            }

            t = t->tb_next;
        }

        Py_DECREF(strace);
    }

    state::error(s);
    Py_DECREF(exc);
}

PyObject* loadrequest_toobject(const RDLoaderRequest* req) {
    PyObject* pyreq = python::new_simplenamespace();
    PyObject_SetAttrString(pyreq, "path", PyUnicode_FromString(req->path));
    PyObject_SetAttrString(pyreq, "name", PyUnicode_FromString(req->name));
    PyObject_SetAttrString(pyreq, "ext", PyUnicode_FromString(req->ext));
    PyObject_SetAttrString(pyreq, "file", pyfile_frombuffer(req->file));
    return pyreq;
}

RDLoaderRequest loadrequest_fromobject(PyObject* obj) {
    return {
        .path = PyUnicode_AsUTF8(PyObject_GetAttrString(obj, "path")),
        .name = PyUnicode_AsUTF8(PyObject_GetAttrString(obj, "name")),
        .ext = PyUnicode_AsUTF8(PyObject_GetAttrString(obj, "ext")),
        .file = pyfile_asbuffer(PyObject_GetAttrString(obj, "file")),
    };
}

} // namespace redasm::api::python
