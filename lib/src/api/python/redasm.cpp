#include "redasm.h"
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
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(std::isalnum(b));
}

PyObject* is_alpha(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(std::isalpha(b));
}

PyObject* is_lower(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(std::islower(b));
}

PyObject* is_upper(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_upper(b));
}

PyObject* is_digit(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_digit(b));
}

PyObject* is_xdigit(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_xdigit(b));
}

PyObject* is_cntrl(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_cntrl(b));
}

PyObject* is_graph(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_graph(b));
}

PyObject* is_space(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_space(b));
}

PyObject* is_blank(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_blank(b));
}

PyObject* is_print(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_print(b));
}

PyObject* is_punct(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(internal::is_punct(b));
}

PyObject* init(PyObject* /*self*/, PyObject* /*args*/) {
    internal::init(nullptr); // TODO(davide): Handle RDInitParams
    return Py_None;
}

PyObject* deinit(PyObject* /*self*/, PyObject* /*args*/) {
    internal::deinit();
    return Py_None;
}

PyObject* set_loglevel(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    auto level = static_cast<RDLogLevel>(PyLong_AsUnsignedLong(args));
    internal::set_loglevel(level);
    return Py_None;
}

PyObject* add_searchpath(PyObject* /*self*/, PyObject* args) {
    if(!PyUnicode_Check(args)) return python::type_error(args, "string");
    internal::add_searchpath(PyUnicode_AsUTF8(args));
    return Py_None;
}

PyObject* get_searchpaths(PyObject* /*self*/, PyObject* /*args*/) {
    const mm::SearchPaths& sp = internal::get_searchpaths();
    PyObject* res = PyTuple_New(sp.size());

    for(usize i = 0; i < sp.size(); i++)
        PyTuple_SET_ITEM(res, i, PyUnicode_FromString(sp[i].c_str()));

    return res;
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

PyObject* set_userdata(PyObject* /*self*/, PyObject* args) {
    const char* k = nullptr;
    uptr v = 0;
    if(!PyArg_ParseTuple(args, "zK", &k, &v)) return nullptr;
    internal::set_userdata(k, v);
    return Py_None;
}

PyObject* get_userdata(PyObject* /*self*/, PyObject* args) {
    if(!PyUnicode_Check(args)) return python::type_error(args, "string");
    auto ud = internal::get_userdata(PyUnicode_AsUTF8(args));
    if(ud) return PyLong_FromUnsignedLongLong(*ud);
    return Py_None;
}

PyObject* log(PyObject* /*self*/, PyObject* args) {
    if(!PyUnicode_Check(args)) return python::type_error(args, "string");
    internal::log(PyUnicode_AsUTF8(args));
    return Py_None;
}

PyObject* status(PyObject* /*self*/, PyObject* args) {
    if(!PyUnicode_Check(args)) return python::type_error(args, "string");
    internal::status(PyUnicode_AsUTF8(args));
    return Py_None;
}

PyObject* symbolize(PyObject* /*self*/, PyObject* args) {
    if(!PyUnicode_Check(args)) return python::type_error(args, "string");
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
    if(!PyArg_ParseTuple(args, "KK", &fromaddr, &type)) return nullptr;

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
    if(!PyLong_Check(args)) return python::type_error(args, "int");

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
    if(!PyArg_ParseTuple(args, "KK", &toaddr, &type)) return nullptr;

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
    if(!PyUnicode_Check(args)) return python::type_error(args, "string");
    const char* name = PyUnicode_AsUTF8(args);

    return internal::get_address(name).map_or(
        [](RDAddress address) { return PyLong_FromUnsignedLong(address); },
        Py_None);
}

PyObject* get_name(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);
    return PyUnicode_FromString(internal::get_name(address).c_str());
}

PyObject* set_type(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* name = nullptr;
    if(!PyArg_ParseTuple(args, "Kz", &address, &name)) return nullptr;

    return internal::set_typename(address, name)
        .map_or(python::to_object, Py_None);
}

PyObject* set_type_ex(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* name = nullptr;
    usize flags = 0;

    if(!PyArg_ParseTuple(args, "KzK", &address, &name, &flags)) return nullptr;

    return internal::set_typename_ex(address, name, flags)
        .map_or(python::to_object, Py_None);
}

PyObject* set_name(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* name = nullptr;

    if(!PyArg_ParseTuple(args, "Kz", &address, &name)) return nullptr;
    return PyBool_FromLong(internal::set_name(address, name));
}

PyObject* set_name_ex(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* name = nullptr;
    usize flags = 0;

    if(!PyArg_ParseTuple(args, "KzK", &address, &name, &flags)) return nullptr;
    return PyBool_FromLong(internal::set_name_ex(address, name, flags));
}

PyObject* set_function(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    RDAddress address = PyLong_AsUnsignedLongLong(args);
    return PyBool_FromLong(internal::set_function(address));
}

PyObject* set_function_ex(PyObject* /*self*/, PyObject* args) {
    return Py_None;
}

PyObject* set_entry(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* name = nullptr;
    if(!PyArg_ParseTuple(args, "Kz", &address, &name)) return nullptr;

    std::string n;
    if(name) n = name;
    return PyBool_FromLong(internal::set_entry(address, n));
}

PyObject* add_ref(PyObject* /*self*/, PyObject* args) {
    RDAddress fromaddr{}, toaddr{};
    usize type = {};

    if(!PyArg_ParseTuple(args, "KKK", &fromaddr, &toaddr, &type))
        return nullptr;

    internal::add_ref(fromaddr, toaddr, type);
    return Py_None;
}

PyObject* add_problem(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* problem = nullptr;
    if(!PyArg_ParseTuple(args, "Ks", &address, &problem)) return nullptr;
    internal::add_problem(address, problem);
    return Py_None;
}

PyObject* get_bool(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    RDAddress address = PyLong_AsUnsignedLongLong(args);
    if(auto v = internal::get_bool(address); v) return PyBool_FromLong(*v);
    return Py_None;
}

PyObject* get_char(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_char(address); v)
        return PyUnicode_FromStringAndSize(&v.value(), 1);

    return Py_None;
}

PyObject* get_u8(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u8(address); v)
        return PyLong_FromUnsignedLong(*v);

    return Py_None;
}

PyObject* get_u16(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u16(address); v)
        return PyLong_FromUnsignedLong(*v);

    return Py_None;
}

PyObject* get_u32(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u32(address); v)
        return PyLong_FromUnsignedLong(*v);

    return Py_None;
}

PyObject* get_u64(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u64(address); v)
        return PyLong_FromUnsignedLongLong(*v);

    return Py_None;
}

PyObject* get_i8(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i8(address); v) return PyLong_FromLong(*v);

    return Py_None;
}

PyObject* get_i16(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i16(address); v) return PyLong_FromLong(*v);

    return Py_None;
}

PyObject* get_i32(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i32(address); v) return PyLong_FromLong(*v);

    return Py_None;
}

PyObject* get_i64(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i32(address); v) return PyLong_FromLongLong(*v);

    return Py_None;
}

PyObject* get_u16be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u16be(address); v)
        return PyLong_FromUnsignedLong(*v);

    return Py_None;
}

PyObject* get_u32be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u32be(address); v)
        return PyLong_FromUnsignedLong(*v);

    return Py_None;
}

PyObject* get_u64be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_u64be(address); v)
        return PyLong_FromUnsignedLongLong(*v);

    return Py_None;
}

PyObject* get_i16be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i16be(address); v) return PyLong_FromLong(*v);

    return Py_None;
}

PyObject* get_i32be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i32be(address); v) return PyLong_FromLong(*v);

    return Py_None;
}
PyObject* get_i64be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_i64be(address); v) return PyLong_FromLongLong(*v);

    return Py_None;
}

PyObject* get_strz(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::get_strz(address); v)
        return PyUnicode_FromString(v->c_str());

    return Py_None;
}

PyObject* get_str(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    usize n{};

    if(!PyArg_ParseTuple(args, "KK", &address, &n)) return nullptr;

    if(auto v = internal::get_str(address, n); v)
        return PyUnicode_FromString(v->c_str());

    return Py_None;
}

PyObject* get_type(PyObject* /*self*/, PyObject* args) {
    RDAddress address{};
    const char* tname = nullptr;

    if(!PyArg_ParseTuple(args, "Ks*", &address, &tname)) return nullptr;

    if(auto v = internal::get_typename(address, tname); v)
        return python::to_object(*v);

    return Py_None;
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

PyObject* memory_info(PyObject* /*self*/, PyObject* /*args*/) {
    RDMemoryInfo mi;
    internal::memory_info(&mi);

    PyObject* res = python::new_simplenamespace();
    PyObject* ba = PyLong_FromUnsignedLongLong(mi.baseaddress);
    PyObject* endba = PyLong_FromUnsignedLongLong(mi.end_baseaddress);
    PyObject* size = PyLong_FromUnsignedLongLong(mi.size);

    PyObject_SetAttrString(res, "baseaddress", ba);
    PyObject_SetAttrString(res, "end_baseaddress", endba);
    PyObject_SetAttrString(res, "size", size);

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
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(internal::is_address(address)) Py_RETURN_TRUE;

    Py_RETURN_FALSE;
}

PyObject* to_offset(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::to_offset(address); v)
        return PyLong_FromUnsignedLongLong(*v);

    return Py_None;
}

PyObject* to_address(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress offset = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::to_address(offset); v)
        return PyLong_FromUnsignedLongLong(*v);

    return Py_None;
}

PyObject* from_reladdress(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress offset = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::from_reladdress(offset); v)
        return PyLong_FromUnsignedLongLong(*v);

    return Py_None;
}

PyObject* address_to_index(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::address_to_index(address); v)
        return PyLong_FromUnsignedLongLong(*v);

    return Py_None;
}

PyObject* index_to_address(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    usize index = PyLong_AsUnsignedLongLong(args);

    if(auto v = internal::index_to_address(index); v)
        return PyLong_FromUnsignedLongLong(*v);

    return Py_None;
}

PyObject* check_string(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    RDAddress address = PyLong_AsUnsignedLongLong(args);

    RDStringResult r{};
    if(!internal::check_string(address, &r)) return Py_None;

    assume(r.type);

    PyObject* res = python::new_simplenamespace();
    PyObject* type = PyUnicode_FromString(r.type);
    PyObject* value = PyUnicode_FromString(r.value);
    PyObject* totalsize = PyLong_FromUnsignedLongLong(r.totalsize);

    PyObject_SetAttrString(res, "type", type);
    PyObject_SetAttrString(res, "value", value);
    PyObject_SetAttrString(res, "totalsize", totalsize);
    PyObject_SetAttrString(res, "terminated", PyBool_FromLong(r.terminated));

    Py_DECREF(totalsize);
    Py_DECREF(value);
    Py_DECREF(type);
    return res;
}

} // namespace redasm::api::python
