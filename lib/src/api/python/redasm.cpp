#include "redasm.h"
#include "../internal/analyzer.h"
#include "../internal/loader.h"
#include "../internal/memory.h"
#include "../internal/processor.h"
#include "../internal/redasm.h"
#include "common.h"

namespace redasm::api::python {

struct LoaderUserData {
    std::string name;
    PyObject* init;
};

struct AnalyzerUserData {
    std::string name;
    PyObject* isenabled;
    PyObject* execute;
};

struct ProcessorUserData {
    std::string name;
    PyObject* emulate;
    PyObject* rendersegment;
    PyObject* renderfunction;
    PyObject* renderinstruction;
};

PyObject* is_alnum(PyObject* /*self*/, PyObject* args) {
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return std::isalnum(b) ? Py_True : Py_False;
}

PyObject* is_alpha(PyObject* /*self*/, PyObject* args) {
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return std::isalpha(b) ? Py_True : Py_False;
}

PyObject* is_lower(PyObject* /*self*/, PyObject* args) {
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return std::islower(b) ? Py_True : Py_False;
}

PyObject* is_upper(PyObject* /*self*/, PyObject* args) {
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return internal::is_upper(b) ? Py_True : Py_False;
}

PyObject* is_digit(PyObject* /*self*/, PyObject* args) {
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return internal::is_digit(b) ? Py_True : Py_False;
}

PyObject* is_xdigit(PyObject* /*self*/, PyObject* args) {
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return internal::is_xdigit(b) ? Py_True : Py_False;
}

PyObject* is_cntrl(PyObject* /*self*/, PyObject* args) {
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return internal::is_cntrl(b) ? Py_True : Py_False;
}

PyObject* is_graph(PyObject* /*self*/, PyObject* args) {
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return internal::is_graph(b) ? Py_True : Py_False;
}

PyObject* is_space(PyObject* /*self*/, PyObject* args) {
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return internal::is_space(b) ? Py_True : Py_False;
}

PyObject* is_blank(PyObject* /*self*/, PyObject* args) {
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return internal::is_blank(b) ? Py_True : Py_False;
}

PyObject* is_print(PyObject* /*self*/, PyObject* args) {
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return internal::is_print(b) ? Py_True : Py_False;
}

PyObject* is_punct(PyObject* /*self*/, PyObject* args) {
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return internal::is_punct(b) ? Py_True : Py_False;
}

PyObject* init(PyObject* /*self*/, PyObject* /*args*/) {
    internal::init();
    Py_RETURN_NONE;
}

PyObject* deinit(PyObject* /*self*/, PyObject* /*args*/) {
    internal::deinit();
    Py_RETURN_NONE;
}

PyObject* set_loglevel(PyObject* /*self*/, PyObject* args) {
    auto level = static_cast<RDLogLevel>(PyLong_AsUnsignedLong(args));
    internal::set_loglevel(level);
    Py_RETURN_NONE;
}

PyObject* symbolize(PyObject* /*self*/, PyObject* args) {
    const char* s = PyUnicode_AsUTF8(args);
    return PyUnicode_FromString(internal::symbolize(s).c_str());
}

PyObject* get_refs(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);
    std::vector<RDRef> refs = internal::get_refs(address);
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
    const char* name = PyUnicode_AsUTF8(args);

    return internal::get_address(name).map_or(
        [](RDAddress address) { return PyLong_FromUnsignedLong(address); },
        Py_None);
}

PyObject* get_name(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);
    return PyUnicode_FromString(internal::get_name(address).c_str());
}

PyObject* set_type(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* name = nullptr;

    if(!PyArg_ParseTuple(args, "Kz", &address, &name))
        return nullptr;

    return internal::set_type(address, name).map_or(python::to_object, Py_None);
}

PyObject* set_type_as(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char *name = nullptr, *dbname = nullptr;

    if(!PyArg_ParseTuple(args, "Kzz", &address, &name, &dbname))
        return nullptr;

    return internal::set_type_as(address, name, dbname)
        .map_or(python::to_object, Py_None);
}

PyObject* set_name(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* name = nullptr;

    if(!PyArg_ParseTuple(args, "Kz", &address, &name))
        return nullptr;

    return internal::set_name(address, name) ? Py_True : Py_False;
}

PyObject* set_export(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);
    return internal::set_export(address) ? Py_True : Py_False;
}

PyObject* set_import(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);
    return internal::set_import(address) ? Py_True : Py_False;
}

PyObject* set_function(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);
    return internal::set_function(address) ? Py_True : Py_False;
}

PyObject* set_function_as(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* name = nullptr;

    if(!PyArg_ParseTuple(args, "Kz", &address, &name))
        return nullptr;

    return internal::set_function_as(address, name) ? Py_True : Py_False;
}

PyObject* set_entry(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* name = nullptr;

    if(!PyArg_ParseTuple(args, "Kz", &address, &name))
        return nullptr;

    std::string n;
    if(name)
        n = name;

    return internal::set_entry(address, n) ? Py_True : Py_False;
}

PyObject* get_bool(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_bool(address); v)
        return *v ? Py_True : Py_False;

    Py_RETURN_NONE;
}

PyObject* get_char(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_char(address); v)
        return PyUnicode_FromStringAndSize(&v.value(), 1);

    Py_RETURN_NONE;
}

PyObject* get_u8(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u8(address); v)
        return PyLong_FromUnsignedLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_u16(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u16(address); v)
        return PyLong_FromUnsignedLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_u32(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u32(address); v)
        return PyLong_FromUnsignedLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_u64(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u64(address); v)
        return PyLong_FromUnsignedLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_i8(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i8(address); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_i16(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i16(address); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_i32(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i32(address); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_i64(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i32(address); v)
        return PyLong_FromLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_u16be(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u16be(address); v)
        return PyLong_FromUnsignedLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_u32be(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u32be(address); v)
        return PyLong_FromUnsignedLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_u64be(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u64be(address); v)
        return PyLong_FromUnsignedLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_i16be(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i16be(address); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_i32be(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i32be(address); v)
        return PyLong_FromLong(*v);

    Py_RETURN_NONE;
}
PyObject* get_i64be(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i64be(address); v)
        return PyLong_FromLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* get_stringz(PyObject* /*self*/, PyObject* args) {
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
    const char* name = PyUnicode_AsUTF8(args);
    internal::set_processor(name);
    Py_RETURN_NONE;
}

PyObject* register_loader(PyObject* /*self*/, PyObject* args,
                          PyObject* kwargs) {

    static const char* const KW_LIST[] = {
        "name",
        "init",
        nullptr,
    };

    const char* name = nullptr;
    PyObject* initfn = nullptr;

    if(!PyArg_ParseTupleAndKeywords(
           args, kwargs, "sO", const_cast<char**>(KW_LIST), &name, &initfn)) {
        return nullptr;
    }

    Py_INCREF(reinterpret_cast<PyObject*>(initfn));

    auto* userdata = new LoaderUserData{
        name,
        initfn,
    };

    RDLoader loader{};
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
        "name",
        "emulate",
        "render_segment",
        "render_function",
        "render_instruction",
        nullptr,
    };

    const char* name = nullptr;
    PyObject *emulatefn = nullptr, *rendersegmentfn = nullptr;
    PyObject *renderfunctionfn = nullptr, *renderinstructionfn = nullptr;

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "sO|OOO",
                                    const_cast<char**>(KW_LIST), &name,
                                    &emulatefn, &rendersegmentfn,
                                    &renderfunctionfn, &renderinstructionfn)) {
        return nullptr;
    }

    Py_INCREF(emulatefn);
    Py_XINCREF(rendersegmentfn);
    Py_XINCREF(renderfunctionfn);
    Py_XINCREF(renderinstructionfn);

    auto* userdata = new ProcessorUserData{
        name, emulatefn, rendersegmentfn, renderfunctionfn, renderinstructionfn,
    };

    RDProcessor processor{};
    processor.name = userdata->name.c_str();
    processor.userdata = userdata;

    processor.emulate = [](const RDProcessor* self, RDAddress address,
                           RDEmulator* r) -> usize {
        auto* userdata = reinterpret_cast<ProcessorUserData*>(self->userdata);
        PyObject* ret = PyObject_CallNoArgs(userdata->emulate);

        if(ret) {
            if(ret == Py_None)
                return 0;

            usize s = PyLong_AsUnsignedLongLong(ret);
            Py_DECREF(ret);
            return s;
        }

        python::on_error();
        return 0;
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
                                         const RDRendererParams* rp) {
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
        "name", "execute", "description", "flags", "is_enabled", nullptr,
    };

    const char *name = nullptr, *description = nullptr;
    usize flags = ANALYZER_NONE;
    PyObject *executefn = nullptr, *isenabledfn = nullptr;

    if(!PyArg_ParseTupleAndKeywords(
           args, kwargs, "sO|sKO", const_cast<char**>(KW_LIST), &name,
           &executefn, &description, &flags, &isenabledfn)) {
        return nullptr;
    }

    Py_INCREF(executefn);
    Py_XINCREF(isenabledfn);

    auto* userdata = new AnalyzerUserData{
        name,
        isenabledfn,
        executefn,
    };

    RDAnalyzer analyzer{};
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

PyObject* get_ep(PyObject* /*self*/, PyObject* /*args*/) {
    return redasm::api::internal::get_ep().map_or(
        [](RDAddress ep) { return PyLong_FromUnsignedLongLong(ep); }, Py_None);
}

PyObject* get_bits(PyObject* /*self*/, PyObject* /*args*/) {
    return PyLong_FromLong(redasm::api::internal::get_bits());
}

PyObject* set_bits(PyObject* /*self*/, PyObject* args) {
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
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(internal::is_address(address))
        Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}

PyObject* address_to_offset(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::address_to_offset(address); v)
        return PyLong_FromUnsignedLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* offset_to_address(PyObject* /*self*/, PyObject* args) {
    RDAddress offset = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::offset_to_address(offset); v)
        return PyLong_FromUnsignedLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* address_to_index(PyObject* /*self*/, PyObject* args) {
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::address_to_index(address); v)
        return PyLong_FromUnsignedLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* index_to_address(PyObject* /*self*/, PyObject* args) {
    usize index = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::index_to_address(index); v)
        return PyLong_FromUnsignedLongLong(*v);

    Py_RETURN_NONE;
}

PyObject* check_string(PyObject* /*self*/, PyObject* args) {
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
        "n",
        nullptr,
    };

    usize v = {}, n = 0;

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "K|K",
                                    const_cast<char**>(KW_LIST), &v, &n)) {
        return nullptr;
    }

    if(n > 0)
        return PyUnicode_FromString(internal::to_hex_n(v, n).c_str());

    return PyUnicode_FromString(internal::to_hex(v).c_str());
}

PyObject* enqueue(PyObject* self, PyObject* args) {
    internal::enqueue(PyLong_AsUnsignedLongLong(args));
    Py_RETURN_NONE;
}

PyObject* schedule(PyObject* self, PyObject* args) {
    internal::schedule(PyLong_AsUnsignedLongLong(args));
    Py_RETURN_NONE;
}

} // namespace redasm::api::python
