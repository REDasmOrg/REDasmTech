#include "buffer.h"
#include "methods.h"
#include "rdil.h"
#include "stream.h"
#include <Python.h>
#include <filesystem>
#include <string_view>
#include <vector>

#define py_addintconstant(m, c) PyModule_AddIntConstant(m, #c, c)

namespace python {

namespace fs = std::filesystem;

namespace {

constexpr std::string_view PYTHON_PREFIX = "python";

std::vector<std::string> g_initpaths;

// clang-format off
struct PyModuleDef moduledef = { // NOLINT
    PyModuleDef_HEAD_INIT,
    "redasm",
    nullptr,
    -1,
    python::methods,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
};
// clang-format on

void init_constants(PyObject* m) {
    py_addintconstant(m, SP_R);
    py_addintconstant(m, SP_W);
    py_addintconstant(m, SP_X);
    py_addintconstant(m, SP_RW);
    py_addintconstant(m, SP_RX);
    py_addintconstant(m, SP_RWX);
    py_addintconstant(m, SP_WX);
    py_addintconstant(m, AF_RUNONCE);
    py_addintconstant(m, AF_SELECTED);
    py_addintconstant(m, AF_EXPERIMENTAL);

    py_addintconstant(m, DR_READ);
    py_addintconstant(m, DR_WRITE);
    py_addintconstant(m, DR_ADDRESS);
    py_addintconstant(m, CR_CALL);
    py_addintconstant(m, CR_JUMP);

    py_addintconstant(m, SN_NOWARN);
    py_addintconstant(m, SN_IMPORT);
    py_addintconstant(m, SN_FORCE);

    py_addintconstant(m, ST_WEAK);
}

PyMODINIT_FUNC PyInit_redasm() { // NOLINT
    PyType_Ready(&python::memory_type);
    PyType_Ready(&python::file_type);
    PyType_Ready(&python::rdil_type);

    PyObject* m = PyModule_Create(&moduledef);
    PyModule_AddObject(m, "memory", python::pymemory_new());
    PyModule_AddObject(m, "file", python::pyfile_new());
    PyModule_AddObject(m, "rdil", python::pyrdil_new());
    PyModule_AddType(m, &python::filestream_type);
    PyModule_AddType(m, &python::memorystream_type);

    init_constants(m);
    return m;
}

bool init_and_configure() {
    PyImport_AppendInittab("redasm", PyInit_redasm);
    Py_Initialize();

    Py_Initialize();
    if(!Py_IsInitialized()) return false;

    PyObject* syspath = PySys_GetObject("path");

    const char** searchpaths;
    usize n = rd_getsearchpaths(&searchpaths);

    for(usize i = 0; i < n; i++) {
        fs::path rootpath = fs::path{searchpaths[i]} / PYTHON_PREFIX;
        if(!fs::is_directory(rootpath)) continue;

        fs::path initfile = rootpath / "__init__.py";
        if(!fs::is_regular_file(initfile)) continue;

        PyObject* item = PyUnicode_FromString(rootpath.c_str());
        PyList_Append(syspath, item);
        Py_DECREF(item);
        g_initpaths.push_back(initfile.string());
    }

    return true;
}

} // namespace

} // namespace python

void rdplugin_create() {
    if(!python::init_and_configure()) return;

    for(const std::string& init : python::g_initpaths) {
        FILE* fp = std::fopen(init.c_str(), "r");

        if(!fp) {
            // spdlog::error("Cannot open '{}'", init);
            continue;
        }

        // spdlog::info("Executing '{}'", init);
        if(PyRun_SimpleFile(fp, init.c_str()) == -1) python::check_error();

        std::fclose(fp);
    }
}

void rdplugin_destroy() {
    if(Py_IsInitialized()) Py_Finalize();
}
