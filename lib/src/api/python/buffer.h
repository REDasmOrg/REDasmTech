#pragma once

#include <Python.h>

struct RDBuffer;

namespace redasm::api::python {

extern PyTypeObject file_type;
extern PyTypeObject memory_type;

PyObject* pyfile_new();
PyObject* pyfile_frombuffer(RDBuffer* buffer);
RDBuffer* pyfile_asbuffer(PyObject* file);
PyObject* pymemory_new();
PyObject* pymemory_frombuffer(RDBuffer* buffer);
RDBuffer* pymemory_asbuffer(PyObject* memory);

} // namespace redasm::api::python
