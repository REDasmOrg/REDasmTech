#include "common.h"
#include "buffer.h"
#include <sstream>
#include <string>

namespace python {

bool validate_class(PyObject* obj,
                    std::initializer_list<const char*> reqattrs) {
    if(!PyType_Check(obj)) return python::type_error(obj, "class");

    for(const char* attr : reqattrs) {
        if(!PyObject_HasAttrString(obj, attr))
            return python::attr_error(obj, attr);
    }

    return true;
}

bool tuple_to_struct(PyObject* obj, std::vector<RDStructField>& s) {
    if(!PyTuple_Check(obj)) {
        PyErr_SetString(PyExc_TypeError, "Expected a tuple");
        return false;
    }

    Py_ssize_t size = PyTuple_Size(obj);
    s.clear();
    s.reserve(size + 1);

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
        s.emplace_back(RDStructField{ftype, fname});
    }

    s.emplace_back(RDStructField{nullptr, nullptr});
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

PyObject* to_object(const RDValue* v) {
    PyObject* res = Py_None;

    if(rdvalue_islist(v)) {
        usize len = rdvalue_getlength(v);
        if(v->type.n != len) return nullptr;

        if(v->type.id == TID_CHAR ||
           v->type.id == TID_WCHAR) { // (w)char[N]: convert to string
            res = PyUnicode_New(v->type.n, 127);

            for(usize i = 0; i < len; i++) {
                const RDValue& item = slice_at(&v->list, i);

                if(item.ch_v) // Check for '\0'
                    PyUnicode_WriteChar(res, i, item.ch_v);
                else {
                    PyUnicode_Resize(&res, i); // Resize to null-terminator
                    break;
                }
            }
        }
        else {
            res = PyList_New(len);

            for(usize i = 0; i < len; i++) {
                const RDValue& item = slice_at(&v->list, i);
                PyList_SetItem(res, i, python::to_object(&item));
            }
        }
    }
    else if(rdvalue_isstruct(v)) {
        res = python::new_simplenamespace();

        RDValueHNode* it;
        hmap_foreach(it, &v->dict, RDValueHNode, hnode) {
            PyObject* f = python::to_object(&it->value);
            PyObject_SetAttrString(res, it->key, f);
            Py_DECREF(f);
        }
    }
    else if(v->type.id == TID_BOOL)
        res = PyBool_FromLong(v->b_v);
    else if(v->type.id == TID_CHAR || v->type.id == TID_WCHAR)
        res = PyUnicode_FromStringAndSize(&v->ch_v, 1);
    else if(v->type.id == TID_U8)
        res = PyLong_FromUnsignedLong(v->u8_v);
    else if(v->type.id == TID_U16)
        res = PyLong_FromUnsignedLong(v->u16_v);
    else if(v->type.id == TID_U32)
        res = PyLong_FromUnsignedLong(v->u32_v);
    else if(v->type.id == TID_U64)
        res = PyLong_FromUnsignedLong(v->u64_v);
    else if(v->type.id == TID_I8)
        res = PyLong_FromUnsignedLong(v->i8_v);
    else if(v->type.id == TID_I16)
        res = PyLong_FromUnsignedLong(v->i16_v);
    else if(v->type.id == TID_I32)
        res = PyLong_FromUnsignedLong(v->i32_v);
    else if(v->type.id == TID_I64)
        res = PyLong_FromUnsignedLong(v->i64_v);
    else if(v->type.id == TID_STR || v->type.id == TID_WSTR)
        res = PyUnicode_FromString(v->str.data);

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

                // clang-format off
                std::stringstream ss;
                ss << "\n File  \"" << PyUnicode_AsUTF8(fcode->co_filename) << "\""
                      " line " << PyFrame_GetLineNumber(frame) << "," << 
                      " in " << PyUnicode_AsUTF8(fcode->co_name);
                // clang-format on

                s += ss.str();
                frame = PyFrame_GetBack(frame);
            }

            t = t->tb_next;
        }

        Py_DECREF(strace);
    }

    rd_error(s.c_str());
    Py_DECREF(exc);
}

PyObject* loadrequest_toobject(const RDLoaderRequest* req) {
    PyObject* pyreq = python::new_simplenamespace();
    PyObject_SetAttrString(pyreq, "path", PyUnicode_FromString(req->path));
    PyObject_SetAttrString(pyreq, "name", PyUnicode_FromString(req->name));
    PyObject_SetAttrString(pyreq, "ext", PyUnicode_FromString(req->ext));
    PyObject_SetAttrString(pyreq, "file", pybuffer_frombuffer(req->file));
    return pyreq;
}

RDLoaderRequest loadrequest_fromobject(PyObject* obj) {
    RDLoaderRequest req;
    req.path = PyUnicode_AsUTF8(PyObject_GetAttrString(obj, "path"));
    req.name = PyUnicode_AsUTF8(PyObject_GetAttrString(obj, "name"));
    req.ext = PyUnicode_AsUTF8(PyObject_GetAttrString(obj, "ext"));
    req.file = pybuffer_asbuffer(PyObject_GetAttrString(obj, "file"));
    return req;
}

} // namespace python
