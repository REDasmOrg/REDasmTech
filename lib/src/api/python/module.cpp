#include "module.h"
#include "../../error.h"
#include "../../plugins/modulemanager.h"
#include "buffer.h"
#include "common.h"
#include "methods.h"
#include "rdil.h"
#include "stream.h"
#include <Python.h>
#include <cstdio>
#include <filesystem>
#include <fmt/core.h>
#include <redasm/redasm.h>
#include <spdlog/spdlog.h>

namespace redasm::api::python {

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
    // clang-format off
    assume(PyModule_AddIntConstant(m, "SEG_UNKNOWN", SEG_UNKNOWN) == 0);
    assume(PyModule_AddIntConstant(m, "SEG_HASDATA", SEG_HASDATA) == 0);
    assume(PyModule_AddIntConstant(m, "SEG_HASCODE", SEG_HASCODE) == 0);
    assume(PyModule_AddIntConstant(m, "ANA_RUNONCE", AF_RUNONCE) == 0);
    assume(PyModule_AddIntConstant(m, "ANA_SELECTED", AF_SELECTED) == 0);
    assume(PyModule_AddIntConstant(m, "ANA_EXPERIMENTAL", AF_EXPERIMENTAL) == 0);

    assume(PyModule_AddIntConstant(m, "DR_READ", DR_READ) == 0);
    assume(PyModule_AddIntConstant(m, "DR_WRITE", DR_WRITE) == 0);
    assume(PyModule_AddIntConstant(m, "DR_ADDRESS", DR_ADDRESS) == 0);
    assume(PyModule_AddIntConstant(m, "CR_CALL", CR_CALL) == 0);
    assume(PyModule_AddIntConstant(m, "CR_JUMP", CR_JUMP) == 0);

    assume(PyModule_AddIntConstant(m, "SN_NOWARN", SN_NOWARN) == 0);
    assume(PyModule_AddIntConstant(m, "SN_IMPORT", SN_IMPORT) == 0);
    assume(PyModule_AddIntConstant(m, "SN_FORCE", SN_FORCE) == 0);

    assume(PyModule_AddIntConstant(m, "ST_FORCE", ST_WEAK) == 0);
    // clang-format on
}

} // namespace

PyMODINIT_FUNC PyInit_redasm() { // NOLINT
    assume(PyType_Ready(&python::memory_type) == 0);
    assume(PyType_Ready(&python::file_type) == 0);
    assume(PyType_Ready(&python::rdil_type) == 0);

    PyObject* m = PyModule_Create(&moduledef);
    assume(PyModule_AddObject(m, "memory", pymemory_new()) == 0);
    assume(PyModule_AddObject(m, "file", pyfile_new()) == 0);
    assume(PyModule_AddObject(m, "rdil", pyrdil_new()) == 0);
    assume(PyModule_AddType(m, &python::filestream_type) == 0);
    assume(PyModule_AddType(m, &python::memorystream_type) == 0);

    python::init_constants(m);
    return m;
}

bool init() {
    if(Py_IsInitialized()) return true;

    spdlog::info("Initializing Python");
    PyImport_AppendInittab("redasm", PyInit_redasm);
    Py_Initialize();

    if(!Py_IsInitialized()) {
        spdlog::error("Python initialization failed");
        return false;
    }

    PyObject* syspath = PySys_GetObject("path");

    for(const std::string& sp : mm::get_searchpaths()) {
        fs::path rootpath = fs::path{sp} / PYTHON_PREFIX;
        if(!fs::is_directory(rootpath)) continue;

        fs::path initfile = rootpath / "__init__.py";
        if(!fs::is_regular_file(initfile)) continue;

        PyObject* item = PyUnicode_FromString(rootpath.c_str());
        assume(PyList_Append(syspath, item) == 0);
        Py_DECREF(item);
        g_initpaths.push_back(initfile.string());
    }

    return true;
}

void deinit() {
    spdlog::info("Deinitializing Python");

    if(Py_IsInitialized()) Py_Finalize();
}

void main() {
    assume(Py_IsInitialized());

    for(const std::string& init : g_initpaths) {
        FILE* fp = std::fopen(init.c_str(), "r");

        if(!fp) {
            spdlog::error("Cannot open '{}'", init);
            continue;
        }

        spdlog::info("Executing '{}'", init);

        if(PyRun_SimpleFile(fp, init.c_str()) == -1) python::check_error();

        std::fclose(fp);
    }
}

} // namespace redasm::api::python
