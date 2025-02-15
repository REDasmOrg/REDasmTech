#pragma once

#include <Python.h>
#include <redasm/redasm.h>

namespace python {

extern PyTypeObject buffer_type;
extern PyTypeObject file_type;
extern PyTypeObject memory_type;

PyObject* pyfile_new();
PyObject* pymemory_new();
PyObject* pybuffer_frombuffer(RDBuffer* b);
RDBuffer* pybuffer_asbuffer(PyObject* obj);
bool pybuffer_check(PyObject* obj);

} // namespace python
