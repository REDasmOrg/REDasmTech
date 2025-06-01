#include "redasm.h"
#include "common.h"
#include <cctype>
#include <redasm/redasm.h>
#include <string>

namespace python {

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
    return PyBool_FromLong(std::isupper(b));
}

PyObject* is_digit(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(std::isdigit(b));
}

PyObject* is_xdigit(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(std::isxdigit(b));
}

PyObject* is_cntrl(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(std::iscntrl(b));
}

PyObject* is_graph(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(std::isgraph(b));
}

PyObject* is_space(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(std::isspace(b));
}

PyObject* is_blank(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(std::isblank(b));
}

PyObject* is_print(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(std::isprint(b));
}

PyObject* is_punct(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    auto b = static_cast<u8>(PyLong_AsUnsignedLong(args));
    return PyBool_FromLong(std::ispunct(b));
}

PyObject* init(PyObject* /*self*/, PyObject* /*args*/) {
    rd_init(nullptr); // TODO(davide): Handle RDInitParams
    return Py_None;
}

PyObject* deinit(PyObject* /*self*/, PyObject* /*args*/) {
    rd_deinit();
    return Py_None;
}

PyObject* set_loglevel(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    auto level = static_cast<RDLogLevel>(PyLong_AsUnsignedLong(args));
    rd_setloglevel(level);
    return Py_None;
}

PyObject* add_searchpath(PyObject* /*self*/, PyObject* args) {
    if(!PyUnicode_Check(args)) return python::type_error(args, "string");
    rd_addsearchpath(PyUnicode_AsUTF8(args));
    return Py_None;
}

PyObject* get_searchpaths(PyObject* /*self*/, PyObject* /*args*/) {
    const char** paths = nullptr;
    usize n = rd_getsearchpaths(&paths);
    PyObject* res = PyTuple_New(n);

    for(usize i = 0; i < n; i++)
        PyTuple_SET_ITEM(res, i, PyUnicode_FromString(paths[i]));

    return res;
}

PyObject* get_problems(PyObject* /*self*/, PyObject* /*args*/) {
    const RDProblemSlice* problems = rd_getproblems();
    PyObject* res = PyTuple_New(problems->length);

    for(isize i = 0; i < problems->length; i++) {
        PyObject* item = python::new_simplenamespace();
        PyObject* itemaddr =
            PyLong_FromUnsignedLongLong(slice_at(problems, i).address);
        PyObject* itemprob =
            PyUnicode_FromString(slice_at(problems, i).problem);
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
    unsigned long long v = 0;
    if(!PyArg_ParseTuple(args, "sK", &k, &v)) return nullptr;
    rd_setuserdata(k, v);
    return Py_None;
}

PyObject* get_userdata(PyObject* /*self*/, PyObject* args) {
    if(!PyUnicode_Check(args)) return python::type_error(args, "string");
    uptr ud;
    if(rd_getuserdata(PyUnicode_AsUTF8(args), &ud))
        return PyLong_FromUnsignedLongLong(ud);
    return Py_None;
}

PyObject* log(PyObject* /*self*/, PyObject* args) {
    if(!PyUnicode_Check(args)) return python::type_error(args, "string");
    rd_log(PyUnicode_AsUTF8(args));
    return Py_None;
}

PyObject* status(PyObject* /*self*/, PyObject* args) {
    if(!PyUnicode_Check(args)) return python::type_error(args, "string");
    rd_status(PyUnicode_AsUTF8(args));
    return Py_None;
}

PyObject* symbolize(PyObject* /*self*/, PyObject* args) {
    if(!PyUnicode_Check(args)) return python::type_error(args, "string");
    const char* s = PyUnicode_AsUTF8(args);
    return PyUnicode_FromString(rd_symbolize(s));
}

PyObject* get_refsfrom(PyObject* /*self*/, PyObject* args) {
    const RDRef* refs = nullptr;
    usize n = rd_getrefsfrom(PyLong_AsUnsignedLongLong(args), &refs);
    PyObject* res = PyTuple_New(n);

    for(usize i = 0; i < n; i++) {
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
    unsigned long long fromaddr{};
    size_t type{};
    if(!PyArg_ParseTuple(args, "Kn", &fromaddr, &type)) return nullptr;

    const RDRef* refs = nullptr;
    usize n = rd_getrefsfromtype(fromaddr, type, &refs);
    PyObject* res = PyTuple_New(n);

    for(usize i = 0; i < n; i++) {
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

    const RDRef* refs = nullptr;
    usize n = rd_getrefsto(PyLong_AsUnsignedLongLong(args), &refs);
    PyObject* res = PyTuple_New(n);

    for(usize i = 0; i < n; i++) {
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
    unsigned long long toaddr{};
    size_t type{};
    if(!PyArg_ParseTuple(args, "Kn", &toaddr, &type)) return nullptr;

    const RDRef* refs = nullptr;
    usize n = rd_getrefstotype(toaddr, type, &refs);
    PyObject* res = PyTuple_New(n);

    for(usize i = 0; i < n; i++) {
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

    RDAddress address;
    if(rd_getaddress(name, &address)) return PyLong_FromUnsignedLong(address);
    return Py_None;
}

PyObject* get_name(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    const char* n = rd_getname(PyLong_AsUnsignedLongLong(args));
    return n ? PyUnicode_FromString(n) : Py_None;
}

PyObject* set_type(PyObject* /*self*/, PyObject* args) {
    unsigned long long address{};
    const char* name = nullptr;
    if(!PyArg_ParseTuple(args, "Ks", &address, &name)) return nullptr;

    RDValue v;
    PyObject* obj = Py_None;

    if(rd_settypename(address, name, &v)) {
        obj = python::to_object(&v);
        rdvalue_destroy(&v);
    }

    return obj;
}

PyObject* set_type_ex(PyObject* /*self*/, PyObject* args) {
    unsigned long long address{};
    const char* name = nullptr;
    size_t flags{};
    if(!PyArg_ParseTuple(args, "Ksn", &address, &name, &flags)) return nullptr;

    RDValue v;
    PyObject* obj = Py_None;

    if(rd_settypename_ex(address, name, flags, &v)) {
        obj = python::to_object(&v);
        rdvalue_destroy(&v);
    }

    return obj;
}

PyObject* set_name(PyObject* /*self*/, PyObject* args) {
    unsigned long long address{};
    const char* name = nullptr;
    if(!PyArg_ParseTuple(args, "Kz", &address, &name)) return nullptr;
    return PyBool_FromLong(rd_setname(address, name));
}

PyObject* set_name_ex(PyObject* /*self*/, PyObject* args) {
    unsigned long long address{};
    const char* name = nullptr;
    size_t flags{};

    if(!PyArg_ParseTuple(args, "Kzn", &address, &name, &flags)) return nullptr;
    return PyBool_FromLong(rd_setname_ex(address, name, flags));
}

PyObject* set_function(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);
    return PyBool_FromLong(rd_setfunction(address));
}

PyObject* set_function_ex(PyObject* /*self*/, PyObject* args) {
    return Py_None;
}

PyObject* set_entry(PyObject* /*self*/, PyObject* args) {
    unsigned long long address{};
    const char* name = nullptr;
    if(!PyArg_ParseTuple(args, "Kz", &address, &name)) return nullptr;
    return PyBool_FromLong(rd_setentry(address, name));
}

PyObject* add_ref(PyObject* /*self*/, PyObject* args) {
    unsigned long long fromaddr{}, toaddr{};
    size_t type = {};

    if(!PyArg_ParseTuple(args, "KKn", &fromaddr, &toaddr, &type))
        return nullptr;

    rd_addref(fromaddr, toaddr, type);
    return Py_None;
}

PyObject* add_problem(PyObject* /*self*/, PyObject* args) {
    unsigned long long address{};
    const char* problem = nullptr;
    if(!PyArg_ParseTuple(args, "Ks", &address, &problem)) return nullptr;
    rd_addproblem(address, problem);
    return Py_None;
}

PyObject* get_bool(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    bool v;
    if(rd_getbool(address, &v)) return PyBool_FromLong(v);
    return Py_None;
}

PyObject* get_char(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    char v;
    if(rd_getchar(address, &v)) return PyUnicode_FromStringAndSize(&v, 1);

    return Py_None;
}

PyObject* get_u8(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    u8 v;
    if(rd_getu8(address, &v)) return PyLong_FromUnsignedLong(v);
    return Py_None;
}

PyObject* get_u16(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    u16 v;
    if(rd_getu16(address, &v)) return PyLong_FromUnsignedLong(v);
    return Py_None;
}

PyObject* get_u32(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    u32 v;
    if(rd_getu32(address, &v)) return PyLong_FromUnsignedLong(v);
    return Py_None;
}

PyObject* get_u64(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    u64 v;
    if(rd_getu64(address, &v)) return PyLong_FromUnsignedLongLong(v);
    return Py_None;
}

PyObject* get_i8(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    i8 v;
    if(rd_geti8(address, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* get_i16(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    i16 v;
    if(rd_geti16(address, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* get_i32(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    i32 v;
    if(rd_geti32(address, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* get_i64(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    i32 v;
    if(rd_geti32(address, &v)) return PyLong_FromLongLong(v);
    return Py_None;
}

PyObject* get_u16be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    u16 v;
    if(rd_getu16be(address, &v)) return PyLong_FromUnsignedLong(v);
    return Py_None;
}

PyObject* get_u32be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    u32 v;
    if(rd_getu32be(address, &v)) return PyLong_FromUnsignedLong(v);
    return Py_None;
}

PyObject* get_u64be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    u64 v;
    if(rd_getu64be(address, &v)) return PyLong_FromUnsignedLongLong(v);
    return Py_None;
}

PyObject* get_i16be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    i16 v;
    if(rd_geti16be(address, &v)) return PyLong_FromLong(v);
    return Py_None;
}

PyObject* get_i32be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    i32 v;
    if(rd_geti32be(address, &v)) return PyLong_FromLong(v);
    return Py_None;
}
PyObject* get_i64be(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    i64 v;
    if(rd_geti64be(address, &v)) return PyLong_FromLongLong(v);
    return Py_None;
}

PyObject* get_strz(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");
    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    const char* v = rd_getstrz(address);
    if(v) return PyUnicode_FromString(v);
    return Py_None;
}

PyObject* get_str(PyObject* /*self*/, PyObject* args) {
    unsigned long long address{};
    size_t n{};
    if(!PyArg_ParseTuple(args, "Kn", &address, &n)) return nullptr;

    const char* v = rd_getstr(address, n);
    if(v) return PyUnicode_FromString(v);
    return Py_None;
}

PyObject* get_type(PyObject* /*self*/, PyObject* args) {
    unsigned long long address{};
    const char* tname = nullptr;
    if(!PyArg_ParseTuple(args, "Ks", &address, &tname)) return nullptr;

    RDValueOpt v = rd_gettypename(address, tname);
    PyObject* obj = Py_None;

    if(v.ok) {
        obj = python::to_object(&v.value);
        rdvalue_destroy(&v.value);
    }

    return obj;
}

PyObject* get_entries(PyObject* /*self*/, PyObject* /*args*/) {
    RDAddress* entries;
    usize n = rd_getentries(&entries);

    PyObject* tuple = PyTuple_New(n);
    for(usize i = 0; i < n; i++)
        PyTuple_SET_ITEM(tuple, i, PyLong_FromUnsignedLongLong(entries[i]));
    return tuple;
}

PyObject* map_file(PyObject* /*self*/, PyObject* args) {
    unsigned long long offset{}, start{}, end{};
    if(!PyArg_ParseTuple(args, "KKK", &offset, &start, &end)) return nullptr;
    return PyBool_FromLong(rd_mapfile_n(offset, start, end));
}

PyObject* map_file_n(PyObject* /*self*/, PyObject* args) {
    unsigned long long offset{}, base{};
    size_t n{};

    if(!PyArg_ParseTuple(args, "KKn", &offset, &base, &n)) return nullptr;
    return PyBool_FromLong(rd_mapfile_n(offset, base, n));
}

PyObject* add_segment(PyObject* /*self*/, PyObject* args) {
    Py_buffer name;
    unsigned long long address{}, endaddress{};
    unsigned int perm{}, bits{};

    // Allow embedded null characters
    if(!PyArg_ParseTuple(args, "s*KKII", &name, &address, &endaddress, &perm,
                         &bits)) {
        return nullptr;
    }

    PyObject* res = PyBool_FromLong(rd_addsegment(
        reinterpret_cast<char*>(name.buf), address, endaddress, perm, bits));
    PyBuffer_Release(&name);
    return res;
}

PyObject* add_segment_n(PyObject* /*self*/, PyObject* args) {
    Py_buffer name;
    unsigned long long address{};
    size_t n;
    unsigned int perm{}, bits{};

    // Allow embedded null characters
    if(!PyArg_ParseTuple(args, "s*KnII", &name, &address, &n, &perm, &bits))
        return nullptr;

    PyObject* res = PyBool_FromLong(rd_addsegment_n(
        reinterpret_cast<char*>(name.buf), address, n, perm, bits));
    PyBuffer_Release(&name);
    return res;
}

PyObject* is_address(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    unsigned long long address = PyLong_AsUnsignedLongLong(args);
    return PyBool_FromLong(rd_isaddress(address));
}

PyObject* to_offset(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    unsigned long long address = PyLong_AsUnsignedLongLong(args);
    RDOffset offset{};

    if(rd_tooffset(address, &offset))
        return PyLong_FromUnsignedLongLong(offset);

    return Py_None;
}

PyObject* to_address(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    unsigned long long offset = PyLong_AsUnsignedLongLong(args);
    RDAddress address{};

    if(rd_toaddress(offset, &address))
        return PyLong_FromUnsignedLongLong(address);

    return Py_None;
}

PyObject* check_string(PyObject* /*self*/, PyObject* args) {
    if(!PyLong_Check(args)) return python::type_error(args, "int");

    unsigned long long address = PyLong_AsUnsignedLongLong(args);

    RDStringResult r{};
    if(!rd_checkstring(address, &r)) return Py_None;

    PyObject* res = python::new_simplenamespace();
    PyObject* type = PyUnicode_FromString(r.type.def->name);
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

} // namespace python
