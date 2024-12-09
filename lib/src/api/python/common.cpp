#include "common.h"
#include "../../error.h"
#include "../../state.h"

namespace redasm::api::python {

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

void check_error() {
    if(!PyErr_Occurred())
        return;

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

        if(!s.empty())
            s += '\n';

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

} // namespace redasm::api::python
