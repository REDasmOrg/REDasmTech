#include "module.h"
#include "../../error.h"
#include "../../state.h"
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

constexpr std::string_view CONFIG_ROOT = "redasm_cfg";
constexpr std::string_view REPO_NAME = "deps";

std::string g_rootpath;
std::string g_repopath;
std::string g_initfilepath;

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
    assume(PyModule_AddIntConstant(m, "ANA_NONE", ANA_NONE) == 0);
    assume(PyModule_AddIntConstant(m, "ANA_RUNONCE", ANA_RUNONCE) == 0);
    assume(PyModule_AddIntConstant(m, "ANA_SELECTED", ANA_SELECTED) == 0);
    assume(PyModule_AddIntConstant(m, "ANA_EXPERIMENTAL", ANA_EXPERIMENTAL) == 0);

    assume(PyModule_AddIntConstant(m, "DR_READ", DR_READ) == 0);
    assume(PyModule_AddIntConstant(m, "DR_WRITE", DR_WRITE) == 0);
    assume(PyModule_AddIntConstant(m, "DR_ADDRESS", DR_ADDRESS) == 0);
    assume(PyModule_AddIntConstant(m, "CR_FLOW", CR_FLOW) == 0);
    assume(PyModule_AddIntConstant(m, "CR_CALL", CR_CALL) == 0);
    assume(PyModule_AddIntConstant(m, "CR_JUMP", CR_JUMP) == 0);

    assume(PyModule_AddIntConstant(m, "SN_DEFAULT", SN_DEFAULT) == 0);
    assume(PyModule_AddIntConstant(m, "SN_NOWARN", SN_NOWARN) == 0);
    assume(PyModule_AddIntConstant(m, "SN_IMPORT", SN_IMPORT) == 0);
    assume(PyModule_AddIntConstant(m, "SN_WEAK", SN_WEAK) == 0);
    assume(PyModule_AddIntConstant(m, "SN_FORCE", SN_FORCE) == 0);
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
    spdlog::info("Initializing Python");

    g_rootpath = (fs::current_path() / CONFIG_ROOT).string();
    g_repopath = (fs::path{g_rootpath} / REPO_NAME).string();

    if(!fs::exists(g_rootpath)) {
        spdlog::warn("'{}' not found", g_rootpath);
        return false;
    }

    g_initfilepath = (fs::path{g_rootpath} / "init.py").string();

    if(!fs::exists(g_initfilepath)) {
        spdlog::warn("'{}' not found", g_initfilepath);
        return false;
    }

    PyImport_AppendInittab("redasm", PyInit_redasm);
    Py_Initialize();

    // Add search paths
    PyObject* syspath = PySys_GetObject("path");
    PyList_Insert(syspath, 0, PyUnicode_FromString(g_repopath.c_str()));
    PyList_Insert(syspath, 0, PyUnicode_FromString(g_rootpath.c_str()));
    return true;
}

void deinit() {
    spdlog::info("Deinitializing Python");

    if(Py_IsInitialized())
        Py_Finalize();
}

void main() {
    assume(Py_IsInitialized());

    FILE* fp = std::fopen(g_initfilepath.c_str(), "r");

    if(!fp) {
        spdlog::error("Cannot open '{}'", g_initfilepath);
        return;
    }

    spdlog::info("Executing '{}'", g_initfilepath);

    if(PyRun_SimpleFileEx(fp, g_initfilepath.c_str(), true)) {
        state::error(fmt::format("Error Executing '{}'", g_initfilepath));
        python::on_error();
    }
}

} // namespace redasm::api::python
