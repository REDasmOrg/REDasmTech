#include "rdil.h"
#include "../../context.h"
#include "../../rdil/rdil.h"
#include "../../state.h"
#include "common.h"

#define REDASM_REGOP(op)                                                       \
    val = PyLong_FromUnsignedLong(RDIL_##op);                                  \
    PyDict_SetItemString(rdil_type.tp_dict, #op, val);                         \
    Py_DECREF(val);

namespace redasm::api::python {

struct PyRDIL {
    PyObject_HEAD
};

namespace {

PyObject* ilexpr_to_pyobject(const rdil::ILExpr* e) {
    if(!e)
        Py_RETURN_NONE;

    PyObject* obj = python::new_simplenamespace();
    PyObject* op = PyLong_FromUnsignedLongLong(e->op);
    PyObject_SetAttrString(obj, "op", op);
    Py_DECREF(op);

    switch(e->op) {
        case RDIL_ADD:
        case RDIL_SUB:
        case RDIL_MUL:
        case RDIL_DIV:
        case RDIL_AND:
        case RDIL_OR:
        case RDIL_XOR:
        case RDIL_EQ:
        case RDIL_NE:
        case RDIL_LT:
        case RDIL_LE:
        case RDIL_GT:
        case RDIL_GE: {
            PyObject* l = python::ilexpr_to_pyobject(e->l);
            PyObject* r = python::ilexpr_to_pyobject(e->r);
            PyObject_SetAttrString(obj, "l", l);
            PyObject_SetAttrString(obj, "r", r);
            Py_DECREF(r);
            Py_DECREF(l);
            break;
        }

        case RDIL_NOT:
        case RDIL_CALL:
        case RDIL_INT:
        case RDIL_GOTO:
        case RDIL_MEM:
        case RDIL_PUSH:
        case RDIL_POP: {
            PyObject* u = python::ilexpr_to_pyobject(e->u);
            PyObject_SetAttrString(obj, "u", u);
            Py_DECREF(u);
            break;
        }

        case RDIL_IF: {
            PyObject* cond = python::ilexpr_to_pyobject(e->cond);
            PyObject* t = python::ilexpr_to_pyobject(e->t);
            PyObject* f = python::ilexpr_to_pyobject(e->f);
            PyObject_SetAttrString(obj, "cond", cond);
            PyObject_SetAttrString(obj, "t", t);
            PyObject_SetAttrString(obj, "f", f);
            Py_DECREF(f);
            Py_DECREF(t);
            Py_DECREF(cond);
            break;
        }

        case RDIL_COPY: {
            PyObject* dst = python::ilexpr_to_pyobject(e->dst);
            PyObject* src = python::ilexpr_to_pyobject(e->src);
            PyObject_SetAttrString(obj, "dst", dst);
            PyObject_SetAttrString(obj, "src", src);
            Py_DECREF(src);
            Py_DECREF(dst);
            break;
        }

        case RDIL_RET: {
            PyObject* cond = python::ilexpr_to_pyobject(e->cond);
            PyObject_SetAttrString(obj, "cond", cond);
            Py_DECREF(cond);
            break;
        }

        case RDIL_CNST: {
            PyObject* val = PyLong_FromUnsignedLongLong(e->u_value);
            PyObject_SetAttrString(obj, "val", val);
            Py_DECREF(val);
            break;
        }

        case RDIL_VAR: {
            PyObject* addr = PyLong_FromUnsignedLongLong(e->address);
            PyObject_SetAttrString(obj, "addr", addr);
            Py_DECREF(addr);
            break;
        }

        case RDIL_REG: {
            PyObject* reg = PyUnicode_FromString(e->reg);
            PyObject_SetAttrString(obj, "reg", reg);
            Py_DECREF(reg);
            break;
        }

        case RDIL_SYM: {
            PyObject* sym = PyUnicode_FromString(e->sym);
            PyObject_SetAttrString(obj, "sym", sym);
            Py_DECREF(sym);
            break;
        }

        default: break;
    }

    return obj;
}

PyObject* rdil_createfunction(PyRDIL* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    const Function* f = state::context->address_to_index(address)
                            .map([](MIndex index) {
                                return state::context->index_to_function(index);
                            })
                            .value();

    if(f) {
        rdil::ILExprList exprlist;
        rdil::generate(*f, exprlist);

        PyObject* res = PyTuple_New(exprlist.size());

        for(auto it = exprlist.begin(); it != exprlist.end(); it++) {
            usize i = std::distance(exprlist.begin(), it);
            auto address = state::context->index_to_address(it->first);
            assume(address.has_value());

            PyObject* addr = PyLong_FromUnsignedLongLong(*address);
            PyObject* item = python::new_simplenamespace();
            PyObject_SetAttrString(item, "address", addr);
            PyObject_SetAttrString(item, "expr",
                                   python::ilexpr_to_pyobject(it->second));

            PyTuple_SET_ITEM(res, i, item);
            Py_DECREF(addr);
        }

        return res;
    }

    Py_RETURN_NONE;
}

// clang-format off
PyMethodDef rdil_methods[] = {
    {"create_function", reinterpret_cast<PyCFunction>(python::rdil_createfunction), METH_O, nullptr},
    {nullptr},
};
// clang-format on

} // namespace

PyTypeObject rdil_type = []() {
    PyTypeObject t{PyVarObject_HEAD_INIT(nullptr, 0)};
    t.tp_name = "redasm.RDIL";
    t.tp_basicsize = sizeof(PyRDIL);
    t.tp_flags = Py_TPFLAGS_DISALLOW_INSTANTIATION;
    t.tp_methods = python::rdil_methods;

    return t;
}();

PyObject* pyrdil_new() {
    auto* obj = reinterpret_cast<PyObject*>(PyObject_New(PyRDIL, &rdil_type));
    PyObject* val = nullptr;

    // clang-format off
    REDASM_REGOP(INVALID);
    REDASM_REGOP(UNKNOWN);
    REDASM_REGOP(NOP);
    REDASM_REGOP(REG); REDASM_REGOP(CNST); REDASM_REGOP(VAR); REDASM_REGOP(SYM);
    REDASM_REGOP(ADD); REDASM_REGOP(SUB); REDASM_REGOP(MUL); REDASM_REGOP(DIV); REDASM_REGOP(MOD);
    REDASM_REGOP(AND); REDASM_REGOP(OR); REDASM_REGOP(XOR); REDASM_REGOP(NOT);
    REDASM_REGOP(LSL); REDASM_REGOP(LSR); REDASM_REGOP(ASL); REDASM_REGOP(ASR);
    REDASM_REGOP(ROL); REDASM_REGOP(ROR);
    REDASM_REGOP(MEM); REDASM_REGOP(COPY);
    REDASM_REGOP(IF); REDASM_REGOP(GOTO); REDASM_REGOP(CALL); REDASM_REGOP(RET);
    REDASM_REGOP(EQ); REDASM_REGOP(NE); REDASM_REGOP(LT); REDASM_REGOP(LE); REDASM_REGOP(GT); REDASM_REGOP(GE);
    REDASM_REGOP(PUSH); REDASM_REGOP(POP);
    REDASM_REGOP(INT);
    // clang-format on

    return obj;
}

} // namespace redasm::api::python
