#include "redasm.h"
#include "../internal/analyzer.h"
#include "../internal/loader.h"
#include "../internal/memory.h"
#include "../internal/processor.h"
#include "../internal/redasm.h"
#include "common.h"

namespace redasm::api::python {

struct LoaderUserData {
    std::string id;
    std::string name;
    PyObject* init;
};

struct AnalyzerUserData {
    std::string id;
    std::string name;
    PyObject* isenabled;
    PyObject* execute;
};

struct ProcessorUserData {
    std::string id;
    std::string name;
    PyObject* emulate;
    PyObject* rendersegment;
    PyObject* renderfunction;
    PyObject* renderinstruction;
};

PyObject* is_alnum(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(std::isalnum(b));
}

PyObject* is_alpha(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(std::isalpha(b));
}

PyObject* is_lower(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(std::islower(b));
}

PyObject* is_upper(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_upper(b));
}

PyObject* is_digit(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_digit(b));
}

PyObject* is_xdigit(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_xdigit(b));
}

PyObject* is_cntrl(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_cntrl(b));
}

PyObject* is_graph(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_graph(b));
}

PyObject* is_space(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_space(b));
}

PyObject* is_blank(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_blank(b));
}

PyObject* is_print(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_print(b));
}

PyObject* is_punct(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_punct(b));
}

PyObject* init(PyObject* /*self*/, PyObject* /*args*/) {
    internal::init(nullptr); // TODO(davide): Handle RDInitParams
    Py_RETURN_NONE;
}

PyObject* deinit(PyObject* /*self*/, PyObject* /*args*/) {
    internal::deinit();
    Py_RETURN_NONE;
}

PyObject* set_loglevel(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    auto level = static_cast<RDLogLevel>(PyLong_AsUnsignedLong(args));
    internal::set_loglevel(level);
    Py_RETURN_NONE;
}

PyObject* get_problems(PyObject* /*self*/, PyObject* /*args*/) {
    std::vector<RDProblem> problems = internal::get_problems();
    PyObject* res = PyTuple_New(problems.size());

    for(usize i = 0; i < problems.size(); i++) {
        PyObject* item = python::new_simplenamespace();
        PyObject* itemaddr = PyLong_FromUnsignedLongLong(problems[i].address);
        PyObject* itemprob = PyUnicode_FromString(problems[i].problem);
        PyObject_SetAttrString(item, "address", itemaddr);
        PyObject_SetAttrString(item, "problem", itemprob);
        PyTuple_SET_ITEM(res, i, item);
        Py_DECREF(itemaddr);
        Py_DECREF(itemprob);
    }

    return res;
}

PyObject* log(PyObject* /*self*/, PyObject* args) {
    if(!PyUnicode_Check(args))
        return python::type_error(args, "string");

    internal::log(PyUnicode_AsUTF8(args));
    Py_RETURN_NONE;
}

PyObject* status(PyObject* /*self*/, PyObject* args) {
    if(!PyUnicode_Check(args))
        return python::type_error(args, "string");

    internal::status(PyUnicode_AsUTF8(args));
    Py_RETURN_NONE;
}

PyObject* symbolize(PyObject* /*self*/, PyObject* args) {
    if(!PyUnicode_Check(args))
        return python::type_error(args, "string");

    const char* s = PyUnicode_AsUTF8(args);
    return PyUnicode_FromString(internal::symbolize(s).c_str());
}

PyObject* get_refsfrom(PyObject* /*self*/, PyObject* args) {
    RDAddress fromaddr = PyLong_AsUnsignedLongLong(args);
    std::vector<RDRef> refs = internal::get_refsfrom(fromaddr);
    PyObject* res = PyTuple_New(refs.size());

    for(usize i = 0; i < refs.size(); i++) {
        PyObject* item = python::new_simplenamespace();
        PyObject* itemaddr = PyLong_FromUnsignedLongLong(refs[i].address);
        PyObject* itemtype = PyLong_FromUnsignedLongLong(refs[i].type);
        PyObject_SetAttrString(item, "address", itemaddr);
        PyObject_SetAttrString(item, "type", itemtype);
        PyTuple_SET_ITEM(res, i, item);
        Py_DECREF(itemaddr);
        Py_DECREF(itemtype);
    }

    return res;
}

PyObject* get_refsfromtype(PyObject* /*self*/, PyObject* args) {
    RDAddress fromaddr = 0;
    usize type = 0;

    if(!PyArg_ParseTuple(args, "KK", &fromaddr, &type))
        return nullptr;

    std::vector<RDRef> refs = internal::get_refsfromtype(fromaddr, type);
    PyObject* res = PyTuple_New(refs.size());

    for(usize i = 0; i < refs.size(); i++) {
        PyObject* item = python::new_simplenamespace();
        PyObject* itemaddr = PyLong_FromUnsignedLongLong(refs[i].address);
        PyObject* itemtype = PyLong_FromUnsignedLongLong(refs[i].type);
        PyObject_SetAttrString(item, "address", itemaddr);
        PyObject_SetAttrString(item, "type", itemtype);
        PyTuple_SET_ITEM(res, i, item);
        Py_DECREF(itemtype);
        Py_DECREF(itemaddr);
    }

    return res;
}

PyObject* get_refsto(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress toaddr = PyLong_AsUnsignedLongLong(args);
    std::vector<RDRef> refs = internal::get_refsto(toaddr);
    PyObject* res = PyTuple_New(refs.size());

    for(usize i = 0; i < refs.size(); i++) {
        PyObject* item = python::new_simplenamespace();
        PyObject* itemaddr = PyLong_FromUnsignedLongLong(refs[i].address);
        PyObject* itemtype = PyLong_FromUnsignedLongLong(refs[i].type);
        PyObject_SetAttrString(item, "address", itemaddr);
        PyObject_SetAttrString(item, "type", itemtype);
        PyTuple_SET_ITEM(res, i, item);
        Py_DECREF(itemtype);
        Py_DECREF(itemaddr);
    }

    return res;
}

PyObject* get_refstotype(PyObject* /*self*/, PyObject* args) {
    RDAddress toaddr = 0;
    usize type = 0;

    if(!PyArg_ParseTuple(args, "KK", &toaddr, &type))
        return nullptr;

    std::vector<RDRef> refs = internal::get_refstotype(toaddr, type);
    PyObject* res = PyTuple_New(refs.size());

    for(usize i = 0; i < refs.size(); i++) {
        PyObject* item = python::new_simplenamespace();
        PyObject* itemaddr = PyLong_FromUnsignedLongLong(refs[i].address);
        PyObject* itemtype = PyLong_FromUnsignedLongLong(refs[i].type);
        PyObject_SetAttrString(item, "address", itemaddr);
        PyObject_SetAttrString(item, "type", itemtype);
        PyTuple_SET_ITEM(res, i, item);
        Py_DECREF(itemtype);
        Py_DECREF(itemaddr);
    }

    return res;
}

PyObject* get_address(PyObject* /*self*/, PyObject* args) {
    if(!PyUnicode_Check(args))
        return python::type_error(args, "string");

    const char* name = PyUnicode_AsUTF8(args);

    return internal::get_address(name).map_or(
        [](RDAddress address) { return PyLong_FromUnsignedLong(address); },
        Py_None);
}

PyObject* get_name(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);
    return PyUnicode_FromString(internal::get_name(address).c_str());
}

PyObject* set_type(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* name = nullptr;

    if(!PyArg_ParseTuple(args, "Kz", &address, &name))
        return nullptr;

    return internal::set_typename(address, name)
        .map_or(python::to_object, Py_None);
}

PyObject* set_name(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* name = nullptr;

    if(!PyArg_ParseTuple(args, "Kz", &address, &name))
        return nullptr;

    return PyBool_FromLong(internal::set_name(address, name));
}

PyObject* set_name_ex(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* name = nullptr;
    usize flags = 0;

    if(!PyArg_ParseTuple(args, "KzK", &address, &name, &flags))
        return nullptr;

    return PyBool_FromLong(internal::set_name_ex(address, name, flags));
}

PyObject* set_function(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);
    return PyBool_FromLong(internal::set_function(address));
}

PyObject* set_function_ex(PyObject* /*self*/, PyObject* args) {
    Py_RETURN_NONE;
}

PyObject* set_entry(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* name = nullptr;

    if(!PyArg_ParseTuple(args, "Kz", &address, &name))
        return nullptr;

    std::string n;
    if(name)
        n = name;

    return PyBool_FromLong(internal::set_entry(address, n));
}

PyObject* add_ref(PyObject* /*self*/, PyObject* args) {
    RDAddress fromaddr{}, toaddr{};
    usize type = {};

    if(!PyArg_ParseTuple(args, "KKK", &fromaddr, &toaddr, &type))
        return nullptr;

    internal::add_ref(fromaddr, toaddr, type);
    Py_RETURN_NONE;
}

PyObject* add_problem(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* problem = nullptr;

    if(!PyArg_ParseTuple(args, "Ks", &address, &problem))
        return nullptr;

    internal::add_problem(address, problem);
    Py_RETURN_NONE;
}

PyObject* get_bool(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_bool(address); v)
        return *v ? Py_True : Py_False;

    Py_RETURN_NONE;
}

PyObject* get_char(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_char(address); v)
        return PyUnicode_FromStringAndSize(&v.value(), 1);

    Py_RETURN_NONE;
}

PyObject* get_u8(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u8(address); v)
        return PyLong_FromUnsignedLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_u16(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u16(address); v)
        return PyLong_FromUnsignedLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_u32(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u32(address); v)
        return PyLong_FromUnsignedLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_u64(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u64(address); v)
        return PyLong_FromUnsignedLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_i8(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i8(address); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_i16(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i16(address); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_i32(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i32(address); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_i64(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i32(address); v)
        return PyLong_FromLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_u16be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u16be(address); v)
        return PyLong_FromUnsignedLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_u32be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u32be(address); v)
        return PyLong_FromUnsignedLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_u64be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u64be(address); v)
        return PyLong_FromUnsignedLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_i16be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i16be(address); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_i32be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i32be(address); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}
PyObject* get_i64be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i64be(address); v)
        return PyLong_FromLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_stringz(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_stringz(address); v)
        return PyUnicode_FromString(v->c_str());

    Py_RETURN_NONE;
}

PyObject* get_string(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    usize n{};

    if(!PyArg_ParseTuple(args, "KK", &address, &n))
        return nullptr;

    if(auto v = internal::get_string(address, n); v)
        return PyUnicode_FromString(v->c_str());

    Py_RETURN_NONE;
}

PyObject* get_type(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* tname = nullptr;

    if(!PyArg_ParseTuple(args, "Ks*", &address, &tname))
        return nullptr;

    if(auto v = internal::get_type(address, tname); v)
        return python::to_object(*v);

    Py_RETURN_NONE;
}

PyObject* set_processor(PyObject* /*self*/, PyObject* args) {
    if(!PyUnicode_Check(args))
        return python::type_error(args, "string");

    const char* name = PyUnicode_AsUTF8(args);
    internal::set_processor(name);
    Py_RETURN_NONE;
}

PyObject* register_loader(PyObject* /*self*/, PyObject* args,
                          PyObject* kwargs) {

    static const char* const KW_LIST[] = {
        "id",
        "name",
        "init",
        nullptr,
    };

    const char *id = nullptr, *name = nullptr;
    PyObject* initfn = nullptr;

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ssO",
                                    const_cast<char**>(KW_LIST), &id, &name,
                                    &initfn)) {
        return nullptr;
    }

    Py_INCREF(reinterpret_cast<PyObject*>(initfn));

    auto* userdata = new LoaderUserData{
        .id = id,
        .name = name,
        .init = initfn,
    };

    RDLoader loader{};
    loader.id = userdata->id.c_str();
    loader.name = userdata->name.c_str();
    loader.userdata = userdata;

    loader.init = [](RDLoader* self) {
        auto* userdata = reinterpret_cast<LoaderUserData*>(self->userdata);
        PyObject* ret = PyObject_CallNoArgs(userdata->init);

        if(ret) {
            bool ok = PyObject_IsTrue(ret);
            Py_DECREF(ret);
            return ok;
        }

        python::on_error();
        return false;
    };

    loader.free = [](RDLoader* self) {
        auto* userdata = reinterpret_cast<LoaderUserData*>(self->userdata);
        Py_XDECREF(reinterpret_cast<PyObject*>(userdata->init));
        delete userdata;
    };

    api::internal::register_loader(loader);
    Py_RETURN_NONE;
}

PyObject* register_processor(PyObject* /*self*/, PyObject* args,
                             PyObject* kwargs) {
    static const char* const KW_LIST[] = {
        "id",
        "name",
        "emulate",
        "render_segment",
        "render_function",
        "render_instruction",
        nullptr,
    };

    const char *id = nullptr, *name = nullptr;
    PyObject *emulatefn = nullptr, *rendersegmentfn = nullptr;
    PyObject *renderfunctionfn = nullptr, *renderinstructionfn = nullptr;

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "ssO|OOO",
                                    const_cast<char**>(KW_LIST), &id, &name,
                                    &emulatefn, &rendersegmentfn,
                                    &renderfunctionfn, &renderinstructionfn)) {
        return nullptr;
    }

    Py_INCREF(emulatefn);
    Py_XINCREF(rendersegmentfn);
    Py_XINCREF(renderfunctionfn);
    Py_XINCREF(renderinstructionfn);

    auto* userdata = new ProcessorUserData{
        .id = id,
        .name = name,
        .emulate = emulatefn,
        .rendersegment = rendersegmentfn,
        .renderfunction = renderfunctionfn,
        .renderinstruction = renderinstructionfn,
    };

    RDProcessor processor{};
    processor.name = userdata->name.c_str();
    processor.userdata = userdata;

    processor.emulate = [](const RDProcessor* self, RDEmulator* e,
                           const RDInstruction* instr) {
        auto* userdata = reinterpret_cast<ProcessorUserData*>(self->userdata);
        PyObject* ret = PyObject_CallNoArgs(userdata->emulate);
        Py_XDECREF(ret);

        python::on_error();
    };

    if(rendersegmentfn) {
        processor.rendersegment = [](const RDProcessor* self,
                                     const RDRendererParams* rp) {
            auto* userdata =
                reinterpret_cast<ProcessorUserData*>(self->userdata);
            PyObject* ret = PyObject_CallNoArgs(userdata->rendersegment);

            if(ret) {
                bool ok = PyObject_IsTrue(ret);
                Py_DECREF(ret);
                return ok;
            }

            python::on_error();
            return false;
        };
    }

    if(renderfunctionfn) {
        processor.renderfunction = [](const RDProcessor* self,
                                      const RDRendererParams* rp) {
            auto* userdata =
                reinterpret_cast<ProcessorUserData*>(self->userdata);
            PyObject* ret = PyObject_CallNoArgs(userdata->renderfunction);

            if(ret) {
                bool ok = PyObject_IsTrue(ret);
                Py_DECREF(ret);
                return ok;
            }

            python::on_error();
            return false;
        };
    }

    if(renderinstructionfn) {
        processor.renderinstruction = [](const RDProcessor* self,
                                         const RDRendererParams* rp,
                                         const RDInstruction* instr) {
            auto* userdata =
                reinterpret_cast<ProcessorUserData*>(self->userdata);
            PyObject* ret = PyObject_CallNoArgs(userdata->renderinstruction);

            if(ret) {
                bool ok = PyObject_IsTrue(ret);
                Py_DECREF(ret);
                return ok;
            }

            python::on_error();
            return false;
        };
    }

    processor.free = [](RDProcessor* self) {
        auto* userdata = reinterpret_cast<ProcessorUserData*>(self->userdata);
        Py_XDECREF(userdata->emulate);
        Py_XDECREF(userdata->rendersegment);
        Py_XDECREF(userdata->renderfunction);
        Py_XDECREF(userdata->renderinstruction);
        delete userdata;
    };

    api::internal::register_processor(processor);
    Py_RETURN_NONE;
}

PyObject* register_analyzer(PyObject* /*self*/, PyObject* args,
                            PyObject* kwargs) {
    static const char* const KW_LIST[] = {
        "id", "name", "execute", "description", "flags", "is_enabled", nullptr,
    };

    const char *id = nullptr, *name = nullptr, *description = nullptr;
    usize flags = ANA_NONE;
    PyObject *executefn = nullptr, *isenabledfn = nullptr;

    if(!PyArg_ParseTupleAndKeywords(
           args, kwargs, "ssO|sKO", const_cast<char**>(KW_LIST), &id, &name,
           &executefn, &description, &flags, &isenabledfn)) {
        return nullptr;
    }

    Py_INCREF(executefn);
    Py_XINCREF(isenabledfn);

    auto* userdata = new AnalyzerUserData{
        .id = id,
        .name = name,
        .isenabled = isenabledfn,
        .execute = executefn,
    };

    RDAnalyzer analyzer{};
    analyzer.id = userdata->id.c_str();
    analyzer.name = userdata->name.c_str();
    analyzer.flags = flags;
    analyzer.userdata = userdata;

    if(isenabledfn) {
        analyzer.isenabled = [](const RDAnalyzer* self) {
            auto* userdata =
                reinterpret_cast<AnalyzerUserData*>(self->userdata);
            PyObject* ret = PyObject_CallNoArgs(userdata->isenabled);

            if(ret) {
                bool ok = PyObject_IsTrue(ret);
                Py_DECREF(ret);
                return ok;
            }

            python::on_error();
            return false;
        };
    }

    analyzer.execute = [](const RDAnalyzer* self) {
        auto* userdata = reinterpret_cast<AnalyzerUserData*>(self->userdata);
        PyObject* ret = PyObject_CallNoArgs(userdata->execute);

        if(ret)
            Py_DECREF(ret);
        else
            python::on_error();
    };

    analyzer.free = [](const RDAnalyzer* self) {
        auto* userdata = reinterpret_cast<AnalyzerUserData*>(self->userdata);
        Py_XDECREF(userdata->isenabled);
        Py_XDECREF(userdata->execute);
        delete userdata;
    };

    api::internal::register_analyzer(analyzer);
    Py_RETURN_NONE;
}

PyObject* get_entries(PyObject* /*self*/, PyObject* /*args*/) {
    auto entries = redasm::api::internal::get_entries();
    PyObject* tuple = PyTuple_New(entries.size());

    for(auto it = entries.begin(); it != entries.end(); it++) {
        usize idx = std::distance(entries.begin(), it);
        PyTuple_SET_ITEM(tuple, idx, PyLong_FromUnsignedLongLong(*it));
    }

    return tuple;
}

PyObject* get_bits(PyObject* /*self*/, PyObject* /*args*/) {
    return PyLong_FromLong(redasm::api::internal::get_bits());
}

PyObject* set_bits(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    redasm::api::internal::set_bits(PyLong_AsLong(args));
    Py_RETURN_NONE;
}

PyObject* memory_info(PyObject* /*self*/, PyObject* /*args*/) {
    RDMemoryInfo mi;
    internal::memory_info(&mi);

    PyObject* res = python::new_simplenamespace();
    PyObject* ba = PyLong_FromUnsignedLongLong(mi.baseaddress);
    PyObject* endba = PyLong_FromUnsignedLongLong(mi.end_baseaddress);
    PyObject* size = PyLong_FromUnsignedLongLong(mi.size);
    PyObject* bits = PyLong_FromUnsignedLongLong(mi.bits);

    PyObject_SetAttrString(res, "baseaddress", ba);
    PyObject_SetAttrString(res, "end_baseaddress", endba);
    PyObject_SetAttrString(res, "size", size);
    PyObject_SetAttrString(res, "bits", bits);

    Py_DECREF(bits);
    Py_DECREF(size);
    Py_DECREF(endba);
    Py_DECREF(ba);

    return res;
}

PyObject* map_segment(PyObject* /*self*/, PyObject* args) {
    const char* name = nullptr;
    RDAddress address{}, endaddress{};
    RDOffset offset{}, endoffset{};
    usize type{};

    // Allow embedded null characters
    if(!PyArg_ParseTuple(args, "s*KKKKK", &name, &address, &endaddress, &offset,
                         &endoffset, &type)) {
        return nullptr;
    }

    if(internal::map_segment(name, address, endaddress, offset, endoffset,
                             type))
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}

PyObject* map_segment_n(PyObject* /*self*/, PyObject* args) {
    const char* name = nullptr;
    RDAddress address{};
    RDOffset offset{};
    usize asize{}, osize{}, type{};

    // Allow embedded null characters
    if(!PyArg_ParseTuple(args, "s*KKKKK", &name, &address, &asize, &offset,
                         &osize, &type)) {
        return nullptr;
    }

    if(internal::map_segment_n(name, address, asize, offset, osize, type))
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}

PyObject* is_address(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(internal::is_address(address))
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}

PyObject* to_offset(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::to_offset(address); v)
        return PyLong_FromUnsignedLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* to_address(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress offset = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::to_address(offset); v)
        return PyLong_FromUnsignedLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* from_reladdress(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress offset = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::from_reladdress(offset); v)
        return PyLong_FromUnsignedLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* address_to_index(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::address_to_index(address); v)
        return PyLong_FromUnsignedLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* index_to_address(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    usize index = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::index_to_address(index); v)
        return PyLong_FromUnsignedLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* check_string(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args))
        return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    RDStringResult r{};
    if(!internal::check_string(address, &r))
        Py_RETURN_NONE;

    assume(r.type);

    PyObject* res = python::new_simplenamespace();
    PyObject* type = PyUnicode_FromString(r.type);
    PyObject* value = PyUnicode_FromString(r.value);
    PyObject* totalsize = PyLong_FromUnsignedLongLong(r.totalsize);

    PyObject_SetAttrString(res, "type", type);
    PyObject_SetAttrString(res, "value", value);
    PyObject_SetAttrString(res, "totalsize", totalsize);
    PyObject_SetAttrString(res, "terminated",
                           r.terminated ? Py_True : Py_False);

    Py_DECREF(totalsize);
    Py_DECREF(value);
    Py_DECREF(type);
    return res;
}

PyObject* to_hex(PyObject* /*self*/, PyObject* args, PyObject* kwargs) {
    static const char* const KW_LIST[] = {
        "",
        "n",
        nullptr,
    };

    usize v = 0;
    int n = 0;

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "K|i",
                                    const_cast<char**>(KW_LIST), &v, &n)) {
        return nullptr;
    }

    if(n > 0)
        return PyUnicode_FromString(internal::to_hex_n(v, n).c_str());

    return PyUnicode_FromString(internal::to_hex(v).c_str());
}

} // namespace redasm::api::python
