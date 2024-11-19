#include "common.h"
#include "../../context.h"
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
    const typing::Type* t = state::context->types.get_type(v.type);
    PyObject* res = nullptr;

    if(v.is_list()) {
        assume(v.count == v.list.size());

        if(t->is_char()) { // (w)char[N]: convert to string
            assume(v.count == v.list.size());
            res = PyUnicode_New(v.count, 127);

            for(usize i = 0; i < v.list.size(); i++) {
                if(v.list[i].ch_v) // Check for '\0'
                    assume(PyUnicode_WriteChar(res, i, v.list[i].ch_v) == 0);
                else
                    break;
            }
        }
        else {
            res = PyList_New(v.list.size());

            for(usize i = 0; i < v.list.size(); i++)
                PyList_SetItem(res, i, python::to_object(v.list[i]));
        }
    }
    else {
        switch(t->id()) {
            case typing::types::BOOL: res = v.b_v ? Py_True : Py_False; break;

            case typing::types::WCHAR:
            case typing::types::CHAR:
                res = PyUnicode_FromStringAndSize(&v.ch_v, 1);
                break;

            case typing::types::U8:
                res = PyLong_FromUnsignedLong(v.u8_v);
                break;

            case typing::types::U16:
                res = PyLong_FromUnsignedLong(v.u16_v);
                break;

            case typing::types::U32:
                res = PyLong_FromUnsignedLong(v.u32_v);
                break;

            case typing::types::U64:
                res = PyLong_FromUnsignedLongLong(v.u64_v);
                break;

            case typing::types::I8: res = PyLong_FromLong(v.i8_v); break;
            case typing::types::I16: res = PyLong_FromLong(v.i16_v); break;
            case typing::types::I32: res = PyLong_FromLong(v.i32_v); break;
            case typing::types::I64: res = PyLong_FromLongLong(v.i64_v); break;

            case typing::types::STR:
            case typing::types::WSTR:
                res = PyUnicode_FromStringAndSize(v.str.data(), v.str.size());
                break;

            case typing::types::STRUCT: {
                res = python::new_simplenamespace();

                for(const auto& [_, n] : t->dict) {
                    PyObject* field = python::to_object(v.dict.at(n));
                    assume(field);
                    PyObject_SetAttrString(res, n.data(), field);
                    Py_DECREF(field);
                }

                break;
            }

            default: unreachable;
        }
    }

    return res;
}

void on_error() {
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
